#include "stdafx.h"
#include "Player_Spear.h"
#include "InfoSubject.h"
#include "PlayerObserver.h"
#include "CollisionSphere.h"
#include "CollisionManager.h"



CPlayer_Spear::CPlayer_Spear(LPDIRECT3DDEVICE9 pGraphicDev)
: CWeapon(pGraphicDev)
{
}


CPlayer_Spear::~CPlayer_Spear()
{
}

HRESULT CPlayer_Spear::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.5f, 0.5f, 0.5f));
	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(-6.f), D3DXToRadian(168.f), D3DXToRadian(75.f)));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(-4.f, 5.f, 3.5f));


	return NOERROR;
}

int CPlayer_Spear::Update_GameObject(const float & fTimeDelta)
{
	if (nullptr == m_pPlayerWeaponMatrix || nullptr == m_pPlayerMatirx)
		return 0;

	_matrix matWeaponMatrix = (*m_pPlayerWeaponMatrix) * (*m_pPlayerMatirx);
	m_pTransformCom->Set_ParentMatrix(&matWeaponMatrix);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);

	//m_pBoundingBox->Set_ColInfo(m_pStaticMeshCom);
	

	_vec3 vPoint = _vec3(115.f, 1.9423f, 1.7614f);
	D3DXVec3TransformCoord(&vPoint, &vPoint, m_pTransformCom->Get_WorldMatrix());
	_matrix matTemp;
	D3DXMatrixIdentity(&matTemp);
	memcpy(&matTemp.m[3][0], &vPoint, sizeof(_vec3));

	if (nullptr != m_pCollisionSphere)
		m_pCollisionSphere->Set_MatrixWorldCollision(&matTemp);

	return 0;
}

void CPlayer_Spear::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

HRESULT CPlayer_Spear::Create_Sphere(void)
{
	Engine::CComponent*			pComponent = nullptr;
	AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Weapon", 9.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
	m_pCollisionManager->Add_Component(L"Com_Col_Weapon", m_pCollisionSphere);

	return NOERROR;
}

HRESULT CPlayer_Spear::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.StaticMesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_Spear", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CPlayer_Spear::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CPlayer_Spear * CPlayer_Spear::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer_Spear* pInstance = new CPlayer_Spear(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPlayer_Spear Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPlayer_Spear::Free(void)
{
	m_pCollisionManager->Delete_Component(L"Com_Col_Weapon", m_pCollisionSphere);
	CWeapon::Free();
}
