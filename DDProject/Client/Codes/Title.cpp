
#include "stdafx.h"
#include "Title.h"
#include "SceneObserver.h"
#include "InfoSubject.h"

CTitle::CTitle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStaticObject(pGraphicDev)
	, m_pSceneObserver(nullptr)
	, m_fTime(0.f)
{
}


CTitle::~CTitle()
{
}

HRESULT CTitle::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.02f, 0.02f, 0.02f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(1.f, 7.f, 6.f));

	m_pSceneObserver = CSceneObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pSceneObserver);

	return NOERROR;
}

int CTitle::Update_GameObject(const float & fTimeDelta)
{
	m_fTime += fTimeDelta;

	if(m_fTime < 3.f) // 내려오기
		m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &_vec3(0.f, -fTimeDelta, 0.f));

	if (SCENE_MENU_TO_STAGE == m_pSceneObserver->Get_SceneChange()) // 올라가기
		m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &_vec3(0.f, fTimeDelta * 2.f, 0.f));

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);

	return 0;
}

void CTitle::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect);
}

HRESULT CTitle::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	//// For.Shader
	//AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_Mesh", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.StaticMesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_MENU, L"Com_Mesh_Title", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;

}

HRESULT CTitle::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pStaticMeshCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CTitle * CTitle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTitle* pInstance = new CTitle(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CTitle Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CTitle::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pSceneObserver);
	DWORD dwRef = Engine::Safe_Release(m_pSceneObserver);

	CStaticObject::Free();
}
