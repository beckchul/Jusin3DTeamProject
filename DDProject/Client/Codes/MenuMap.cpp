#include "stdafx.h"
#include "MenuMap.h"
#include "Renderer.h"
#include "CollisionManager.h"

USING(Engine)

CMenuMap::CMenuMap(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStaticObject(pGraphicDev)
{
}


CMenuMap::~CMenuMap()
{
}

HRESULT CMenuMap::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return NOERROR;
}

int CMenuMap::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);

	return 0;
}

void CMenuMap::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect);
}

HRESULT CMenuMap::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_MENU, L"Com_Mesh_MenuMap", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CMenuMap::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CMenuMap * CMenuMap::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMenuMap* pInstance = new CMenuMap(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CMenuMap Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CMenuMap::Free(void)
{
	CStaticObject::Free();
}
