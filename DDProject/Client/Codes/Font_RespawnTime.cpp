#include "stdafx.h"
#include "Font_RespawnTime.h"
#include "Renderer.h"
#include "PlayerObserver.h"
#include "InfoSubject.h"

USING(Engine)

CFont_RespawnTime::CFont_RespawnTime(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_pPlayerObserver(nullptr)
	, m_fTime(-1.f)
{
}


CFont_RespawnTime::~CFont_RespawnTime()
{
}

HRESULT CFont_RespawnTime::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f * 1.2f, SCALE_WINCY * 128.f * 1.2f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 486.f, SCALE_WINCY * 376.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	m_pPlayerObserver = CPlayerObserver::Create();
	CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	return NOERROR;
}

int CFont_RespawnTime::Update_GameObject(const float & fTimeDelta)
{
	const PLAYER_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex);
	if (nullptr == pInfo)
		return 0;

	if (PLAYER_DEAD != pInfo->eMainState)
	{
		m_fTime = -1.f;
		return 0;
	}
	else
	{
		if (-1.f == m_fTime)
			m_fTime = 4.f;
	}

	m_fTime -= fTimeDelta;
	if (m_fTime <= -1.f)
		m_fTime = -1.f;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	//CUI::Get_Position();

	return 0;
}

void CFont_RespawnTime::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	pArgEffect->BeginPass(UI_ALPHABLEND);

	m_pTransformCom->SetUp_OnShader(pArgEffect, "g_matWorldViewProj", matView, matProj); // 위치 변경
	m_pTextureCom->SetUp_OnShader(pArgEffect, "g_BaseTexture", (_int)m_fTime);
	pArgEffect->CommitChanges();

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CFont_RespawnTime::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Font_Respawn", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}


CFont_RespawnTime * CFont_RespawnTime::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFont_RespawnTime* pInstance = new CFont_RespawnTime(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CFont_RespawnTime Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CFont_RespawnTime::Free(void)
{
	CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	COrthographicUI::Free();
}
