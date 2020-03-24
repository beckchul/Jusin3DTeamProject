#include "stdafx.h"
#include "Chat_Message.h"
#include "Renderer.h"
#include "InputDev.h"
#include "WaveObserver.h"
#include "InfoSubject.h"
#include "Font_Player.h"
#include "Chat_ID.h"
#include "DataManager.h"

USING(Engine)

CChat_Message::CChat_Message(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex, bool bChatTemp)
	: COrthographicUI(pGraphicDev)
	, m_iClientIdx(iIndex)
	, m_bChatTemp(bChatTemp)
	, m_fTime(0.f)
	, m_iOrder(0)
	, m_pFont_Player(nullptr)
	, m_pChat_ID(nullptr)
{
	ZeroMemory(m_pTextureChat, sizeof(m_pTextureChat));
}


CChat_Message::~CChat_Message()
{
}

HRESULT CChat_Message::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (m_bChatTemp) // 내 채팅
	{
		m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f / 6.f, SCALE_WINCY * 128.f / 6.f, 1.f));
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 42.f, SCALE_WINCY * 754.f, 0.f));
	}
	else // 좌측 채팅
	{
		m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f / 6.f, SCALE_WINCY * 128.f / 6.f, 1.f));
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 125.f, SCALE_WINCY * 365.f, 0.f));
	}
		
	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	// Chat 이미지와 동일해야한다
	CHAR szTemp[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
		'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', ';', '\'', '=', '-', '?', '!', '^', '_',
		' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'};

	for (_uint i = 0; i < strlen(szTemp); ++i)
		m_mapChat.insert(map<CHAR, _int>::value_type(szTemp[i], i));

	m_fTime = 5.f; // 지속 시간

	m_pFont_Player = CFont_Player::Create(m_pGraphicDev, m_iClientIdx);
	if (nullptr == m_pFont_Player)
		return E_FAIL;

	m_pChat_ID = CChat_ID::Create(m_pGraphicDev, m_iClientIdx);
	if (nullptr == m_pChat_ID)
		return E_FAIL;

	return NOERROR;
}

int CChat_Message::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	if (!m_bChatTemp) // 좌측 채팅창의 경우
	{
		m_fTime -= fTimeDelta;
		if (m_fTime < 0.f)
			return 1;

		// 위 아래
		_vec3 vecPos;

		if (g_eCurrentScene != SCENE_STAGE2)
		{
			vecPos = { SCALE_WINCX * 125.f - WINCX / 2.f, -SCALE_WINCY * (365.f + m_iOrder * 30.f) + WINCY / 2.f,0.f };

			// Player 1~4
			m_pFont_Player->Update_GameObject(fTimeDelta);
			m_pFont_Player->Set_Pos(vecPos);
		}
		else // ID로 채팅하기
		{
			vecPos = { SCALE_WINCX * 125.f - WINCX / 2.f, -SCALE_WINCY * (300.f + m_iOrder * 30.f) + WINCY / 2.f,0.f };

			m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vecPos);

			m_pChat_ID->Update_GameObject(fTimeDelta);

			m_pChat_ID->Set_Pos(&(vecPos - _vec3(SCALE_WINCX * 120.f, 0.f, 0.f)));
		}
	}

	//CUI::Get_Position();

	return 0;
}

void CChat_Message::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	pArgEffect->BeginPass(UI_ALPHABLEND);
	
	int iTexture = (m_bChatTemp == TRUE) ? 0 : 1;
	int iInterval = (m_bChatTemp == TRUE) ? 12 : 12;

	for (_uint i = 0; i < strlen(m_szMessage); ++i)
	{
		m_pTextureChat[iTexture]->SetUp_OnShader(pArgEffect, "g_BaseTexture", m_mapChat[m_szMessage[i]]); // 이미지 변경

		m_pTransformCom->Change_WorldMatrix(&_vec3(SCALE_WINCX * iInterval, 0.f, 0.f)); // 글자간 간격
		m_pTransformCom->SetUp_OnShader(pArgEffect, "g_matWorldViewProj", matView, matProj); // 위치 변경
		pArgEffect->CommitChanges();

		m_pBufferCom->Render_Buffer(); // 그리기
	}

	pArgEffect->EndPass();
}

HRESULT CChat_Message::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureChat[0], Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Font_Chat", Engine::CComponent::COM_STATIC, L"Com_Texture");
	AddComponent(m_pTextureChat[1], Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Font_Chat2", Engine::CComponent::COM_STATIC, L"Com_Texture2");
	
	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

CChat_Message * CChat_Message::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex, bool bChatTemp)
{
	CChat_Message* pInstance = new CChat_Message(pGraphicDev, iIndex, bChatTemp);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CChat_Message Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CChat_Message::Free(void)
{
	for (int i = 0; i < 2; ++i)
		Engine::Safe_Release(m_pTextureChat[i]);

	Engine::Safe_Release(m_pFont_Player);
	Engine::Safe_Release(m_pChat_ID);

	COrthographicUI::Free();
}
