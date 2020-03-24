#include "stdafx.h"
#include "Player_Staff.h"
#include "InfoSubject.h"
#include "PlayerObserver.h"

#include "CollisionSphere.h"
#include "CollisionManager.h"

#include "Object_Manager.h"
#include "Missile.h"
#include "EffectMgr.h"

CPlayer_Staff::CPlayer_Staff(LPDIRECT3DDEVICE9 pGraphicDev)
: CWeapon(pGraphicDev)
{ 
}


CPlayer_Staff::~CPlayer_Staff()
{
}

void CPlayer_Staff::AttackMissile(_vec3 vDir, _int iAttPower)
{
	_vec3 vPoint;

	memcpy(&vPoint, &m_pEffectPoint->m[3][0], sizeof(_vec3));
	Engine::CGameObject*		pGameObject = nullptr;

	D3DXVec3Normalize(&vDir, &(vDir - vPoint));
	pGameObject = CMissile::Create(m_pGraphicDev, vPoint, vDir, MAGE_MISSILE);

	((CMissile*)pGameObject)->Set_Damage(iAttPower);
	if (FAILED(Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
		return;

	Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Mage_CommonAtt_Flare", m_pEffectPoint);
}

HRESULT CPlayer_Staff::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.15f, 0.15f, 0.15f));
	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(-6.f), D3DXToRadian(168.f), D3DXToRadian(75.f)));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(1.2f, -2.7f, 1.4f));

	//Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_TrackParticleTest", m_pEffectPoint);


	return NOERROR;
}

int CPlayer_Staff::Update_GameObject(const float & fTimeDelta)
{
	if (nullptr == m_pPlayerWeaponMatrix || nullptr == m_pPlayerMatirx)
		return 0;

	_matrix matWeaponMatrix = (*m_pPlayerWeaponMatrix) * (*m_pPlayerMatirx);
	m_pTransformCom->Set_ParentMatrix(&matWeaponMatrix);

	Engine::CGameObject::Update_GameObject(fTimeDelta);
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);


	_vec3 vPoint = _vec3(90.f, 1.9423f, 1.7614f);		
	D3DXVec3TransformCoord(&vPoint, &vPoint, m_pTransformCom->Get_WorldMatrix());
	D3DXMatrixIdentity(m_pEffectPoint);
	memcpy(&m_pEffectPoint->m[3][0], &vPoint, sizeof(_vec3));

	return 0;
}

void CPlayer_Staff::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

HRESULT CPlayer_Staff::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.StaticMesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_Staff", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;

}

HRESULT CPlayer_Staff::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CPlayer_Staff * CPlayer_Staff::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer_Staff* pInstance = new CPlayer_Staff(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPlayer_Staff Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPlayer_Staff::Free(void)
{
	CWeapon::Free();
}
