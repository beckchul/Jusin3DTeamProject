#include "stdafx.h"
#include "Crystal.h"
#include "CollisionManager.h"

CCrystal::CCrystal(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStaticObject(pGraphicDev)
	, m_pCollisionSphere(nullptr)
	, m_pCollisionManager(CCollisionManager::GetInstance())
{
	m_pCollisionManager->Add_Ref();
}


CCrystal::~CCrystal()
{
}

HRESULT CCrystal::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.1f, 0.1f, 0.1f));

	return NOERROR;
}

int CCrystal::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);
	_matrix matWorld = *m_pTransformCom->Get_WorldMatrix();
	matWorld.m[3][1] += 20.f;
	m_pCollisionSphere->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Rotation(Engine::CTransform::ANGLE_Y, fTimeDelta * 0.8f);

	return 0;
}

void CCrystal::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

HRESULT CCrystal::Add_Component(void)
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
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_Crystal", Engine::CComponent::COM_STATIC, L"Com_Mesh");
	
	
	AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Crystal", 1000.f, m_pTransformCom->Get_WorldMatrix(), 0.1f);
	m_pCollisionManager->Add_Component(L"Com_Col_Crystal", m_pCollisionSphere);

	return NOERROR;

}

HRESULT CCrystal::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CCrystal * CCrystal::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCrystal* pInstance = new CCrystal(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CCrystal Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CCrystal::Free(void)
{
	m_pCollisionManager->Delete_Component(L"Com_Col_Crystal", m_pCollisionSphere);
	Engine::Safe_Release(m_pCollisionSphere);
	Engine::Safe_Release(m_pCollisionManager);

	CStaticObject::Free();
}
