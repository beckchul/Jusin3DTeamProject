#include "stdafx.h"
#include "Chat.h"
#include "Renderer.h"
#include "InputDev.h"
#include "ClientNetwork.h"
#include "Chat_Message.h"

USING(Engine)

CChat::CChat(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_bCharMode(FALSE)
	, m_pInputDev(CInputDev::GetInstance())
	, m_pTempChatMessage(nullptr)
{
	strcpy_s(m_szTemp, "");

	ZeroMemory(m_Alphabet_KeyArray, sizeof(BYTE) * 26);
	ZeroMemory(m_Number_KeyArray, sizeof(BYTE) * 10);

	m_pInputDev->Add_Ref();
}

CChat::~CChat()
{
}

HRESULT CChat::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 2048.f / 2.f, SCALE_WINCY * 64.f / 2.f, 1.f)); // 크기
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 511.f, SCALE_WINCY * 751.f, 1.f)); // 위치

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	m_pTempChatMessage = CChat_Message::Create(m_pGraphicDev, 1, true); // 자기 자신의 채팅창
	if (nullptr == m_pTempChatMessage)
		return E_FAIL;

	BYTE Array_Num[10] = { DIK_0, DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8, DIK_9 };
	memcpy(m_Number_KeyArray, Array_Num, sizeof(Array_Num));
	BYTE Array_Alphabet[26] = { DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, DIK_F, DIK_G, DIK_H, DIK_I, DIK_J, DIK_K,
		DIK_L, DIK_M, DIK_N, DIK_O, DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T, DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z };
	memcpy(m_Alphabet_KeyArray, Array_Alphabet, sizeof(Array_Alphabet));

	return NOERROR;
}

int CChat::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	KeyCheck();

	if (nullptr != m_pRendererCom)
	{
		if (m_bCharMode) // Enter Key 눌렸을때만
		{
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

			// 자신의 채팅창
			m_pTempChatMessage->Set_Message(m_szTemp);
			m_pTempChatMessage->Update_GameObject(fTimeDelta);
		}
	}

	// 좌측 채팅창
	int iOrder = 0;
	list<CChat_Message*>::iterator iter_begin = m_listChatMessage.begin();
	list<CChat_Message*>::iterator iter_end = m_listChatMessage.end();
	for (; iter_begin != iter_end;)
	{
		(*iter_begin)->Set_Order(iOrder++);

		if (1 == (*iter_begin)->Update_GameObject(fTimeDelta))
		{
			Engine::Safe_Release(*iter_begin);
			iter_begin = m_listChatMessage.erase(iter_begin);
		}
		else
			++iter_begin;
	}
		
	return 0;
}

void CChat::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHABLEND);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

void CChat::Get_ServerChat(Engine::NETDATA tData)
{
	CChat_Message* pMessage = CChat_Message::Create(m_pGraphicDev, tData.iIndex);
	if (nullptr == pMessage)
		return;

	pMessage->Set_Message(tData.szChatMessage);
	m_listChatMessage.push_back(pMessage);
}

void CChat::KeyCheck(void)
{
	if (g_eCurrentScene == SCENE_STAGE2 && g_eLastMan_Progress == LASTMAN_WAIT)
		return;

	if (m_pInputDev->OnceKeyDown(DIK_RETURN, true))
	{
		if (!m_bCharMode) // 채팅 모드로 진입
		{
			m_pInputDev->Set_KeyDisEnabled(); // 다른 키는 안먹도록
			m_bCharMode = true;

			return;
		}

		else // 서버로 메세지 보내기
		{
			if (strcmp(m_szTemp, "")) // 무언가 입력되었을 때
			{
				strncpy_s(m_szTemp, m_szTemp, 15); // 버퍼의 크기가 16이니까 최대 15개까지 가능하다.

				Engine::NETDATA tData;
				ZeroMemory(&tData, sizeof(tData));
				tData.chKey = 72;
				tData.eMessage = Engine::NETMSG_CLIENT_SEND_CHATTING;
				strcpy_s(tData.szChatMessage, m_szTemp);
				CClientNetwork::GetInstance()->SendMsg(tData);

				//CObjMgr::GetInstance()->PlaySound(L"Chat.WAV", CHANNEL_CHAT);

				if (!strcmp(m_szTemp, "change 1"))
				{
					if (g_iClientIndex != 1)
					{
						cout << "1번과 교체" << endl;
						CClientNetwork::GetInstance()->Send_ChangeMessage(g_iClientIndex, 1);
					}
				}
				else if (!strcmp(m_szTemp, "change 2"))
				{
					if (g_iClientIndex != 2)
					{
						cout << "2번과 교체" << endl;
						CClientNetwork::GetInstance()->Send_ChangeMessage(g_iClientIndex, 2);
					}

				}
				else if (!strcmp(m_szTemp, "change 3"))
				{
					if (g_iClientIndex != 3)
					{
						cout << "3번과 교체" << endl;
						CClientNetwork::GetInstance()->Send_ChangeMessage(g_iClientIndex, 3);
					}
				}
			}

			m_pInputDev->Set_KeyEnabled();
			m_bCharMode = false;

			strcpy_s(m_szTemp, "");
		}
	}

	if (m_bCharMode) // 채팅창 활성화중
	{
		if (strlen(m_szTemp) > 1000)
			return;

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
	}
}

HRESULT CChat::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	pComponent = m_pRendererCom = ((Engine::CRenderer*)m_pComponentMgr->Clone_Component(SCENE_STATIC, L"Com_Renderer"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", pComponent));
	m_pRendererCom->Add_Ref();

	// For.Transform
	pComponent = m_pTransformCom = ((Engine::CTransform*)m_pComponentMgr->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->Add_Ref();

	// For.Texture
	pComponent = m_pTextureCom = ((Engine::CTexture*)m_pComponentMgr->Clone_Component(SCENE_STATIC, L"Com_Texture_Chat"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->Add_Ref();

	// For.Buffer
	pComponent = m_pBufferCom = ((Engine::CRect_Texture*)m_pComponentMgr->Clone_Component(SCENE_STATIC, L"Com_Buffer_RcTex"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->Add_Ref();

	return NOERROR;
}

HRESULT CChat::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 0);
	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CChat * CChat::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CChat* pInstance = new CChat(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CChat Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CChat::Free(void)
{
	Engine::Safe_Release(m_pInputDev);

	Engine::Safe_Release(m_pTempChatMessage);

	for_each(m_listChatMessage.begin(), m_listChatMessage.end(), CRelease_Single());

	COrthographicUI::Free();
}
