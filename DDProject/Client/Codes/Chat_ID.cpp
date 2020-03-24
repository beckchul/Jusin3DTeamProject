#include "stdafx.h"
#include "Chat_ID.h"
#include "Renderer.h"
#include "DataManager.h"

USING(Engine)

CChat_ID::CChat_ID(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
	: COrthographicUI(pGraphicDev)
	, m_iClientIdx(iIndex)
	, m_fTime(0.f)
{
}


CChat_ID::~CChat_ID()
{
}

HRESULT CChat_ID::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f / 6.f, SCALE_WINCX * 128.f / 6.f, 1.f));
	
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
		' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };

	for (_uint i = 0; i < strlen(szTemp); ++i)
		m_mapChat.insert(map<CHAR, _int>::value_type(szTemp[i], i));

	m_fTime = 5.f; // 지속 시간

	m_szMessage = CDataManager::GetInstance()->Get_Player_ID(m_iClientIdx);

	return NOERROR;
}

int CChat_ID::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	m_fTime -= fTimeDelta;
	if (m_fTime < 0.f)
		return 1;

	//CUI::Get_Position();

	return 0;
}

void CChat_ID::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	pArgEffect->BeginPass(UI_ALPHABLEND);

	int iInterval = 12;

	for (_uint i = 0; i < m_szMessage.length(); ++i)
	{
		m_pTextureCom->SetUp_OnShader(pArgEffect, "g_BaseTexture", m_mapChat[m_szMessage[i]]); // 이미지 변경

		m_pTransformCom->Change_WorldMatrix(&_vec3(SCALE_WINCX * iInterval, 0.f, 0.f)); // 글자간 간격
		m_pTransformCom->SetUp_OnShader(pArgEffect, "g_matWorldViewProj", matView, matProj); // 위치 변경
		pArgEffect->CommitChanges();

		m_pBufferCom->Render_Buffer(); // 그리기
	}

	pArgEffect->EndPass();
}

HRESULT CChat_ID::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Font_Chat", Engine::CComponent::COM_STATIC, L"Com_Texture");
	
	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

CChat_ID * CChat_ID::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
{
	CChat_ID* pInstance = new CChat_ID(pGraphicDev, iIndex);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CChat_ID Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CChat_ID::Free(void)
{

	COrthographicUI::Free();
}
