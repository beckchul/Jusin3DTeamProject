#include "stdafx.h"
#include "CrossBowArrow.h"
#include "CollisionManager.h"

CCrossBow_Arrow::CCrossBow_Arrow(LPDIRECT3DDEVICE9 pGraphicDev)
: CCollisionObject(pGraphicDev)
{
}

CCrossBow_Arrow::~CCrossBow_Arrow(void)
{
}

HRESULT CCrossBow_Arrow::Ready_GameObject(_vec3 vPosition, _vec3 _vDir, _vec3 vAngle)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	m_tInfo = INFO(0, 0, 0, 0, 15, false, 300.f, 0.f, 0.f);

	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &vAngle);
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPosition);
	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.5f, 0.5f, 0.5f));

	m_vDir = _vDir;
	float fAngle = atan2f(m_vDir.z, m_vDir.x);
	m_pTransformCom->SetRotation(CTransform::ANGLE_Y, -fAngle);
	
	Engine::CGameObject::Update_GameObject(0.f);
	m_pCollisionManager->Pick_Terrain(&vPosition, &m_vDir, m_vDestPosition);

	m_pCollisionSphere->Set_MatrixWorldCollision(m_pTransformCom->Get_WorldMatrix());

	return NOERROR;
}

int CCrossBow_Arrow::Update_GameObject(const float& fTimeDelta)
{
	if (m_tInfo.bDead)
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);
	m_pCollisionSphere->Set_Damage(m_tInfo.iAttPower);

	m_pCollisionSphere->Set_MatrixWorldCollision(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);


	_vec3 vPostion = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION) + m_vDir * fTimeDelta * m_tInfo.fSpeedX;
	m_pTransformCom->Set_Information(Engine::CTransform::INFO_POSITION, &vPostion);
	Check_Collision();

	return 0;
}

void CCrossBow_Arrow::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

void CCrossBow_Arrow::Check_Collision(void)
{
	if (m_tInfo.bDead)
		return;

	auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonBody");
	for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
	{
		if (m_pCollisionSphere->Check_Collision((CCollisionSphere*)(*iter)))
		{
			m_tInfo.bDead = true;
			return;
		}
	}

	_vec3 vPostion = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
	if (D3DXVec3Length(&(m_vDestPosition - vPostion)) < 5.f)
		m_tInfo.bDead = true;
}

HRESULT CCrossBow_Arrow::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.StaticMesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_CrossBowArrow", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Weapon", 10.f, m_pTransformCom->Get_WorldMatrix(), 0.5f);
	m_pCollisionManager->Add_Component(L"Com_Col_Weapon", m_pCollisionSphere);

	return NOERROR;
}

HRESULT CCrossBow_Arrow::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CCrossBow_Arrow* CCrossBow_Arrow::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, _vec3 _vDir, _vec3 vAngle)
{
	CCrossBow_Arrow* pInstance = new CCrossBow_Arrow(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPosition, _vDir, vAngle)))
	{
		MessageBox(0, L"CCrossBow_Arrow Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CCrossBow_Arrow::Free(void)
{
	m_pCollisionManager->Delete_Component(L"Com_Col_Weapon", m_pCollisionSphere);
	CCollisionObject::Free();
}
