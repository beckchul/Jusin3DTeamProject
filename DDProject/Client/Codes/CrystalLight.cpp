
#include "stdafx.h"
#include "CrystalLight.h"
#include "DataManager.h"

CCrystalLight::CCrystalLight(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStaticObject(pGraphicDev)
{
}


CCrystalLight::~CCrystalLight()
{
}

HRESULT CCrystalLight::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.1f, 0.1f, 0.1f));

	return NOERROR;
}

int CCrystalLight::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);

	return 0;
}

void CCrystalLight::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

HRESULT CCrystalLight::Add_Component(void)
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
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STAGE,
		Engine::CDataManager::GetInstance()->Get_ObjName()[12]->szObjectName, Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;

}

HRESULT CCrystalLight::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pStaticMeshCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	pEffect->SetVector("g_CrystalTorchColor", (_vec4*)&m_Color);


	return NOERROR;
}

CCrystalLight * CCrystalLight::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCrystalLight* pInstance = new CCrystalLight(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CCrystalLight Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CCrystalLight::Free(void)
{
	CStaticObject::Free();
}
