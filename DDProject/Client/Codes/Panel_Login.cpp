#include "stdafx.h"
#include "Panel_Login.h"
#include "Renderer.h"
#include "InputDev.h"
#include "Chat_Message.h"
#include "ClientNetwork.h"
#include "Text_Info.h"
#include "UI_Check.h"
#include "Object_Manager.h"

USING(Engine)

CPanel_Login::CPanel_Login(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_pInputDev(CInputDev::GetInstance())
	, m_fEnterTime(0.f)
	, m_pTempChatMessage(nullptr)
	, m_bSubmit(false)
	, m_pCurrentId(nullptr)
{
	m_pInputDev->Add_Ref(); 
	
	strcpy_s(m_szTemp, "");

	ZeroMemory(m_Alphabet_KeyArray, sizeof(BYTE) * 26);
	ZeroMemory(m_Number_KeyArray, sizeof(BYTE) * 10);
}

CPanel_Login::~CPanel_Login()
{
}

HRESULT CPanel_Login::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 512.f, SCALE_WINCY * 256.f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 495.f, SCALE_WINCY * -128.f, 1.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	BYTE Array_Num[10] = { DIK_0, DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8, DIK_9 };
	memcpy(m_Number_KeyArray, Array_Num, sizeof(Array_Num));
	BYTE Array_Alphabet[26] = { DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, DIK_F, DIK_G, DIK_H, DIK_I, DIK_J, DIK_K,
		DIK_L, DIK_M, DIK_N, DIK_O, DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T, DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z };
	memcpy(m_Alphabet_KeyArray, Array_Alphabet, sizeof(Array_Alphabet));

	m_pTempChatMessage = CChat_Message::Create(m_pGraphicDev, 1, true); // 자기 자신의 채팅창
	if (nullptr == m_pTempChatMessage)
		return E_FAIL;

	m_pTempChatMessage->Set_Pos(&_vec3(SCALE_WINCX * 374.f - WINCX / 2.f, SCALE_WINCY * -122.f + WINCY / 2.f, 0.f));

	m_pCurrentId = CChat_Message::Create(m_pGraphicDev, 1, true); // 입력된 ID값
	if (nullptr == m_pTempChatMessage)
		return E_FAIL;

	m_pCurrentId->Set_Pos(&_vec3(SCALE_WINCX * 400.f - WINCX / 2.f, SCALE_WINCY * -288.f + WINCY / 2.f, 0.f));

	m_pUICheck = CUI_Check::Create(m_pGraphicDev);
	if (nullptr == m_pUICheck)
		return E_FAIL;

	return NOERROR;
}

int CPanel_Login::Update_GameObject(const float & fTimeDelta)
{
	if (m_pTransformCom->Get_Info(CTransform::INFO_POSITION).y > -SCALE_WINCY * 193.f + WINCY / 2.f)
		m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &_vec3(0.f, -fTimeDelta * 100.f, 0.f));

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_fEnterTime -= fTimeDelta;

	if (m_bDead)
		return 1;

	KeyCheck();

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);
		
		// 자신의 채팅창
		m_pTempChatMessage->Set_Message(m_szTemp);
		m_pTempChatMessage->Update_GameObject(fTimeDelta);

		if (m_bSubmit)
		{
			m_pCurrentId->Update_GameObject(fTimeDelta);
			m_pUICheck->Update_GameObject(fTimeDelta);
		}
			
	}

	//CUI::Get_Position();

	return 0;
}

void CPanel_Login::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pTextureCom->SetUp_OnShader(pArgEffect, "g_BaseTexture", 1);

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHATESETBLEND);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();

	m_pTextureCom->SetUp_OnShader(pArgEffect, "g_BaseTexture", 0);

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHATESETBLEND);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CPanel_Login::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Panel_Login", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CPanel_Login::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

void CPanel_Login::KeyCheck()
{
	if ((m_pInputDev->StayKeyDown(DIK_DELETE) && m_bSubmit) || g_eLastMan_Progress == LASTMAN_ROUND_WAIT)
		m_bDead = true;
		
	if (m_pInputDev->OnceKeyDown(DIK_RETURN, true))
	{
		if (strcmp(m_szTemp, "") && m_fEnterTime < 3.f) // 무언가 입력되었을 때
		{
			strncpy_s(m_szTemp, m_szTemp, 10); // 10글자 제한
			
			char szTemp[1024] = "id = ";
			strcat_s(szTemp, m_szTemp);
			m_pCurrentId->Set_Message(szTemp);
			m_bSubmit = true;

			Engine::CGameObject* pGameObject = CText_Info::Create(m_pGraphicDev);
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE2, L"LAYER_UI", pGameObject);
	
			Engine::NETDATA tData;
			tData.chKey = 72;
			tData.eMessage = NETMSG_CLIENT_SEND_ID;
			tData.iIndex = g_iClientIndex;
			strcpy_s(tData.szChatMessage, m_szTemp);
			CClientNetwork::GetInstance()->SendMsg(tData);

			m_fEnterTime = 3.f; // 여러번 못누르게
			strcpy_s(m_szTemp, "");
		}
	}

	if (m_pInputDev->StayKeyDown(DIK_LSHIFT, true) || m_pInputDev->StayKeyDown(DIK_RSHIFT, true))
	{
		if (m_pInputDev->OnceKeyDown(DIK_SLASH, true)) // ?
			strcat_s(m_szTemp, "?");

		if (m_pInputDev->OnceKeyDown(DIK_1, true)) // !
			strcat_s(m_szTemp, "!");

		if (m_pInputDev->OnceKeyDown(DIK_6, true)) // ^
			strcat_s(m_szTemp, "^");

		if (m_pInputDev->OnceKeyDown(DIK_MINUS, true)) // _
			strcat_s(m_szTemp, "_");
	}

	for (int i = 0; i < 26; ++i) // 'a' to 'z'
	{
		if (m_pInputDev->OnceKeyDown(m_Alphabet_KeyArray[i], true))
		{
			char charBuff[] = { char(i + 65 + 32), '\0' }; // 소문자로 변환

			strcat_s(m_szTemp, charBuff);
		}
	}

	for (int i = 0; i < 10; ++i) // '0' to '9'
	{
		if (m_pInputDev->OnceKeyDown(m_Number_KeyArray[i], true))
		{
			char charBuff[] = { char(i + 48), '\0' };

			strcat_s(m_szTemp, charBuff);
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_SPACE, true)) // 스페이스
		strcat_s(m_szTemp, " ");
	if (m_pInputDev->OnceKeyDown(DIK_PERIOD, true)) // '.'
		strcat_s(m_szTemp, ".");
	if (m_pInputDev->OnceKeyDown(DIK_SEMICOLON, true)) // ';'
		strcat_s(m_szTemp, ";");
	if (m_pInputDev->OnceKeyDown(DIK_APOSTROPHE, true)) // '''
		strcat_s(m_szTemp, "'");
	if (m_pInputDev->OnceKeyDown(DIK_EQUALS, true)) // '='
		strcat_s(m_szTemp, "=");
	if (m_pInputDev->OnceKeyDown(DIK_MINUS, true)) // -
		strcat_s(m_szTemp, "-");

	if (m_pInputDev->OnceKeyDown(DIK_BACKSPACE, true)) // BackSpace
		strncpy_s(m_szTemp, m_szTemp, strlen(m_szTemp) - 1);

	if(strlen(m_szTemp) > 10) // 10글자 까지만 가능
		strncpy_s(m_szTemp, m_szTemp, strlen(m_szTemp) - 1);
}

CPanel_Login * CPanel_Login::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPanel_Login* pInstance = new CPanel_Login(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPanel_Login Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPanel_Login::Free(void)
{
	Engine::Safe_Release(m_pInputDev);
	
	Engine::Safe_Release(m_pTempChatMessage);
	Engine::Safe_Release(m_pUICheck);
	Engine::Safe_Release(m_pCurrentId);

	COrthographicUI::Free();
}
