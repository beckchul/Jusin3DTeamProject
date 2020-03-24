#include "stdafx.h"
#include "EasterEgg.h"

CEasterEgg::CEasterEgg(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStaticObject(pGraphicDev)
{
}


CEasterEgg::~CEasterEgg()
{
}

HRESULT CEasterEgg::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	//m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.1f, 0.1f, 0.1f));

	return NOERROR;
}

int CEasterEgg::Update_GameObject(const float & fTimeDelta)
{
	if (m_bDead)
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);

	return 0;
}

void CEasterEgg::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

HRESULT CEasterEgg::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.StaticMesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_EasterEgg", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CEasterEgg::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CEasterEgg * CEasterEgg::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEasterEgg* pInstance = new CEasterEgg(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEasterEgg Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEasterEgg::Free(void)
{
	CStaticObject::Free();
}
