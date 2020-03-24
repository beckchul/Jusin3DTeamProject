#include "stdafx.h"
#include "Text_PressStart.h"
#include "Renderer.h"
#include "SceneObserver.h"
#include "InfoSubject.h"

USING(Engine)

CText_PressStart::CText_PressStart(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_fTime(0.f)
	, m_pSceneObserver(nullptr)
{
}


CText_PressStart::~CText_PressStart()
{
}

HRESULT CText_PressStart::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 1024.f / 1.5f, SCALE_WINCY * 128.f / 1.5f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 533.f, SCALE_WINCY * 750.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	m_pSceneObserver = CSceneObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pSceneObserver);

	return NOERROR;
}

int CText_PressStart::Update_GameObject(const float & fTimeDelta)
{
	m_fTime += fTimeDelta;

	if(m_fTime < 3.f) // 올라가기
		m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &_vec3(0.f, 50.f * fTimeDelta, 0.f));

	// 커졌다 작아졌다
	m_pTransformCom->Change_Information(CTransform::INFO_SCALE, &_vec3(cosf(m_fTime), cosf(m_fTime) / 8.f, 0.f));

	if (SCENE_MENU_TO_STAGE == m_pSceneObserver->Get_SceneChange()) // 다시 내려가기
		m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &_vec3(0.f, -150.f * fTimeDelta, 0.f));

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	return 0;
}

void CText_PressStart::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHABLEND);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CText_PressStart::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_MENU, L"Com_Texture_PressStart", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CText_PressStart::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CText_PressStart * CText_PressStart::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CText_PressStart* pInstance = new CText_PressStart(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CText_PressStart Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CText_PressStart::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pSceneObserver);
	DWORD dwRef = Engine::Safe_Release(m_pSceneObserver);

	COrthographicUI::Free();
}
