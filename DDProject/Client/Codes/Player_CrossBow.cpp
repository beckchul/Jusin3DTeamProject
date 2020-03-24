#include "stdafx.h"
#include "Player_CrossBow.h"

CPlayer_CrossBow::CPlayer_CrossBow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CWeapon(pGraphicDev) 
{

}

CPlayer_CrossBow::~CPlayer_CrossBow()
{
}

HRESULT CPlayer_CrossBow::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.15f, 0.15f, 0.15f));
	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(0.f), D3DXToRadian(79.5f), D3DXToRadian(-141.5f)));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(-2.5, -1.5, 1));

	return NOERROR;
}

int CPlayer_CrossBow::Update_GameObject(const float & fTimeDelta)
{
	if (nullptr == m_pPlayerWeaponMatrix || nullptr == m_pPlayerMatirx)
		return 0;

	_matrix matWeaponMatrix = (*m_pPlayerWeaponMatrix) * (*m_pPlayerMatirx);
	m_pTransformCom->Set_ParentMatrix(&matWeaponMatrix);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);

	return 0;
}

void CPlayer_CrossBow::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

HRESULT CPlayer_CrossBow::Add_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	//// For.Shader
	//AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_Mesh", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.StaticMesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_CrossBow", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CPlayer_CrossBow::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CPlayer_CrossBow * CPlayer_CrossBow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer_CrossBow* pInstance = new CPlayer_CrossBow(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPlayer_CrossBow Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPlayer_CrossBow::Free(void)
{
	CWeapon::Free();
}
