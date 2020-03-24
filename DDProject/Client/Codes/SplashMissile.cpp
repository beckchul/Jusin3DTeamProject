#include "stdafx.h"
#include "SplashMissile.h"
#include "CollisionManager.h"

#include "EffectMgr.h"

#include "SplashDamage.h"
#include "Object_Manager.h"
#include "Effect.h"

#include "DataManager.h"
#include "SoundMgr.h"

CSplashMissile::CSplashMissile(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCollisionObject(pGraphicDev) 
{
}

CSplashMissile::~CSplashMissile()
{
}

HRESULT CSplashMissile::Ready_GameObject(_vec3 vPosition, _vec3 _vDir, SPLASH_TYPE eType)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	m_vDir = _vDir;
	m_eType = eType;

	
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPosition);
	Engine::CGameObject::Update_GameObject(0.f);

	m_pCollisionManager->Pick_Terrain(&vPosition, &m_vDir, m_vDestPosition);

	m_pEffectMatrix = new _matrix;
	D3DXMatrixIdentity(m_pEffectMatrix);
	memcpy(&m_pEffectMatrix->m[3][0], &vPosition, sizeof(_vec3));

	
	switch (m_eType)
	{
	case MAGE_FIRETOWER_SPLASH_ATTACK:
		m_tInfo = INFO(0, 0, 0, 0, 70, false, 300.f, 0.f, 0.f);
		m_Templist = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_FireTower_Missile", m_pEffectMatrix, m_vDestPosition);
		break;
	case DEMON_SPLASH_MISSLIE:
		m_tInfo = INFO(0, 0, 0, 0, 70, false, 200.f, 0.f, 0.f);
		m_Templist = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Demon_Fire", m_pEffectMatrix, m_vDestPosition);
		break;
	}

	m_pCollisionSphere->Set_MatrixWorldCollision(m_pTransformCom->Get_WorldMatrix());
	m_pCollisionSphere->Set_CheckCollsion(false);
	return NOERROR;
}

int CSplashMissile::Update_GameObject(const float & fTimeDelta)
{
	if (m_tInfo.bDead)
	{
		m_pCollisionSphere->Set_CheckCollsion(false);

		_vec3 Pos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));


		switch (m_eType)
		{
		case MAGE_FIRETOWER_SPLASH_ATTACK:
		{	
			Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_Templist);
			if (fDistance < 300.f)
			{
				CSoundMgr::GetInstance()->Play_RandomSound(L"Tower_FireballExplosion.ogg", Engine::CHANNEL_TOWER_ATTACK_2, 2, 0.8f);
			}

			// Damage
			Engine::CGameObject*		pGameObject = nullptr;
			pGameObject = CSplashDamage::Create(m_pGraphicDev, m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION), MAGE_FIRETOWER_SPLASH_ATTACK);
			((CSplashDamage*)pGameObject)->Set_Damage(m_tInfo.iAttPower);
			Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject);
		}
		break;
		case DEMON_SPLASH_MISSLIE:
		{
			Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_Templist);

			// Damage
			Engine::CGameObject*		pGameObject = nullptr;
			pGameObject = CSplashDamage::Create(m_pGraphicDev, m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION), DEMON_SPLASH_MISSLIE);
			((CSplashDamage*)pGameObject)->Set_Damage(m_tInfo.iAttPower);
			Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject);
		}
			break;
		}

		return 1;
	}

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pCollisionSphere->Set_MatrixWorldCollision(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);


	_vec3 vPosition = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION) + m_vDir * fTimeDelta * m_tInfo.fSpeedX;
	m_pTransformCom->Set_Information(Engine::CTransform::INFO_POSITION, &vPosition);

	D3DXMatrixIdentity(m_pEffectMatrix);
	memcpy(&m_pEffectMatrix->m[3][0], &vPosition, sizeof(_vec3));

	
	Check_Collision();
	m_pCollisionSphere->Set_CheckCollsion(true);
	return 0;
}

void CSplashMissile::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;
}

void CSplashMissile::Check_Collision(void)
{
	if (m_tInfo.bDead)
		return;
	switch (m_eType)
	{
	case MAGE_FIRETOWER_SPLASH_ATTACK:
	{
		auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonBody");
		for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
		{
			if (m_pCollisionSphere->Check_Collision((CCollisionSphere*)(*iter)))
			{
				m_tInfo.bDead = true;
				return;
			}
		}

	}
	break;
	case DEMON_SPLASH_MISSLIE:
	{
		auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_Body");
		for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
		{
			if (m_pCollisionSphere->Check_Collision((CCollisionSphere*)(*iter)))
			{
				m_tInfo.bDead = true;
				return;
			}
		}
	}
	break;
	}
		_vec3 vPostion = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
		if (D3DXVec3Length(&(m_vDestPosition - vPostion)) < 3.f)
			m_tInfo.bDead = true;
}

HRESULT CSplashMissile::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_NotCheck", 7.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
	//m_pCollisionManager->Add_Component(L"Com_Col_Weapon", m_pCollisionSphere);
	m_pCollisionManager->Add_Component(L"Com_Col_NotCheck", m_pCollisionSphere);
	return NOERROR;
}

HRESULT CSplashMissile::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CSplashMissile* CSplashMissile::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, _vec3 vDir, SPLASH_TYPE eType)
{
	CSplashMissile* pInstance = new CSplashMissile(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPosition, vDir, eType)))
	{
		MessageBox(0, L"CSplashMissile Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CSplashMissile::Free(void)
{
	//m_pCollisionManager->Delete_Component(L"Com_Col_Weapon", m_pCollisionSphere);
	m_pCollisionManager->Delete_Component(L"Com_Col_NotCheck", m_pCollisionSphere);
	Engine::Safe_Delete(m_pEffectMatrix);
	CCollisionObject::Free();
}
