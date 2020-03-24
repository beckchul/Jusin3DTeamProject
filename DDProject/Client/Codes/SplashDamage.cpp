#include "stdafx.h"
#include "SplashDamage.h"
#include "CollisionManager.h"
#include "EffectMgr.h"
#include "Effect.h"

CSplashDamage::CSplashDamage(LPDIRECT3DDEVICE9 pGraphicDev)
: CCollisionObject(pGraphicDev)
, m_fTimeCount(0.f)
, m_fLifeTime(0.f)
{
}

CSplashDamage::~CSplashDamage()
{
}

HRESULT CSplashDamage::Ready_GameObject(_vec3 vPosition, SPLASH_TYPE eType)
{
	m_eType = eType;

	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPosition);
	Engine::CGameObject::Update_GameObject(0.f);

	m_pEffectMatrix = new _matrix;
	memcpy(&m_pEffectMatrix->m[3][0], &vPosition, sizeof(_vec3));


	switch (m_eType)
	{
	//case MAGE_SPLASH_ATTACK:
	//	// 이펙트 구현 되어있는 스킬임
	//	break;
	case MAGE_FIRETOWER_SPLASH_ATTACK:
		Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_FireTower_Missile_Del", &vPosition);
		break;
	case KOBOLD_SPLASH_ATTACK:
		// 구현되었도다
		Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Tower_Destruction", &vPosition);
		break;
	case DEMON_SPLASH_MISSLIE:
		Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_FireTower_Missile_Del", &vPosition);
		break;
	case MAGE_LIGHTNING_TOWER_ATTACK:
		Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_LightningTower_Missile", &vPosition);
		break;
	case  DEMON_SPLASH_ATTACK:
		break;
	}

	m_pCollisionSphere->Set_MatrixWorldCollision(m_pTransformCom->Get_WorldMatrix());
	m_pCollisionSphere->Set_CheckCollsion(false);
	
	return NOERROR;
}

int CSplashDamage::Update_GameObject(const float & fTimeDelta)
{
	m_pCollisionSphere->Set_Damage(m_tInfo.iAttPower);

	if (m_tInfo.bDead)
		return 1;
	
	Engine::CGameObject::Update_GameObject(fTimeDelta);
	m_pCollisionSphere->Set_MatrixWorldCollision(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);

	m_fTimeCount += fTimeDelta * 10.f;
	if (m_fTimeCount >= 1.f)
	{
		m_fTimeCount = 0.f;
		++m_fLifeTime;
	}

	Check_Collision();
	m_pCollisionSphere->Set_CheckCollsion(true);
	return 0;
}

void CSplashDamage::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

}

void CSplashDamage::Check_Collision(void)
{
	if (m_tInfo.bDead)
		return;

	if (m_bCheck)
		m_pCollisionSphere->Set_CheckCollsion(false);

	auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonBody");
	for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
	{
		if (m_pCollisionSphere->Check_Collision((CCollisionSphere*)(*iter)))
		{
			m_bCheck = true;
			break;
		}
	}


	if (m_fLifeTime >= 2.f)
		m_tInfo.bDead = true;
}

HRESULT CSplashDamage::Add_Component(void)
{

	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*	pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");
	
	switch (m_eType)
	{
	case MAGE_SPLASH_ATTACK:
		AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Weapon", 45.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_Weapon", m_pCollisionSphere);
		m_pCollisionSphere->Set_Damage(150);
		break;
	case MAGE_MANA_BOMB_SPLASH_ATTACK:
		AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Weapon", 70.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_Weapon", m_pCollisionSphere);
		m_pCollisionSphere->Set_Damage(600);
		break;
	case MAGE_FIRETOWER_SPLASH_ATTACK:
		AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Weapon", 20.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_Weapon", m_pCollisionSphere);
		break;
	case MAGE_LIGHTNING_TOWER_ATTACK:
		AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Weapon", 60.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_Weapon", m_pCollisionSphere);
		m_pCollisionSphere->Set_Damage(20);
		break;
	case KOBOLD_SPLASH_ATTACK:
		AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_MonWeapon", 30.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_MonWeapon", m_pCollisionSphere);
		break;
	case DEMON_SPLASH_MISSLIE:
		AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_MonWeapon", 20.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_MonWeapon", m_pCollisionSphere);
		break;
	case DEMON_SPLASH_ATTACK:
		AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_MonWeapon", 65.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_MonWeapon", m_pCollisionSphere);
		break;
	}
	return NOERROR;
}

HRESULT CSplashDamage::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CSplashDamage* CSplashDamage::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, SPLASH_TYPE eType)
{
	CSplashDamage* pInstance = new CSplashDamage(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPosition, eType)))
	{
		MessageBox(0, L"CSplashDamage Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CSplashDamage::Free(void)
{
	switch (m_eType)
	{
	case MAGE_SPLASH_ATTACK:
	case MAGE_FIRETOWER_SPLASH_ATTACK:
	case MAGE_MANA_BOMB_SPLASH_ATTACK:
	case MAGE_LIGHTNING_TOWER_ATTACK:
		m_pCollisionManager->Delete_Component(L"Com_Col_Weapon", m_pCollisionSphere);
		break;
	case KOBOLD_SPLASH_ATTACK:
	case DEMON_SPLASH_MISSLIE:
	case DEMON_SPLASH_ATTACK:
		m_pCollisionManager->Delete_Component(L"Com_Col_MonWeapon", m_pCollisionSphere);
		break;
	}
	
	Engine::Safe_Delete(m_pEffectMatrix);
	CCollisionObject::Free();
}
