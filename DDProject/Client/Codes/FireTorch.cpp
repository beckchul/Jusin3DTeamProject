#include "stdafx.h"
#include "FireTorch.h"
#include "Renderer.h"

USING(Engine)

CFireTorch::CFireTorch(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pMeshCom(nullptr)
{
}


CFireTorch::~CFireTorch()
{
}

HRESULT CFireTorch::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.001f, 0.001f, 0.001f));
	m_pTransformCom->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(180.f), 0.f));

	return NOERROR;
}

int CFireTorch::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);

	return 0;
}

void CFireTorch::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	if (FAILED(Set_ConstantTable(pEffect)))
		return;

	pEffect->Begin(nullptr, 0);

	m_pMeshCom->Render_Mesh(pEffect);

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CFireTorch::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_Mesh", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.Mesh
	AddComponent(m_pMeshCom, Engine::CStaticMesh*, SCENE_STAGE, L"Com_Mesh_FireTorch", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CFireTorch::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	return NOERROR;
}

CFireTorch * CFireTorch::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFireTorch* pInstance = new CFireTorch(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CFireTorch Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CFireTorch::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMeshCom);

	Engine::CGameObject::Free();
}
