#include "stdafx.h"
#include "Missile.h"
#include "CollisionManager.h"

#include "Effect.h"
#include "EffectMgr.h"
#include "SoundMgr.h"
#include "DataManager.h"

CMissile::CMissile(LPDIRECT3DDEVICE9 pGraphicDev)
: CCollisionObject(pGraphicDev) 
{
}

CMissile::~CMissile()
{
}

HRESULT CMissile::Ready_GameObject(_vec3 vPosition, _vec3 _vDir, MISSILE_TYPE eType)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	m_eMissileType = eType;
	m_vDir = _vDir;
	m_tInfo = INFO(0, 0, 0, 0, 15, false, 200.f, 0.f, 0.f);
	
	//cout << vPosition.x << " " << vPosition.y << " " << vPosition.z << endl;
	//cout << _vDir.x << " " << _vDir.y << " " << _vDir.z << endl;

	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPosition);
	Engine::CGameObject::Update_GameObject(0.f);

	m_pCollisionManager->Pick_Terrain(&vPosition, &m_vDir, m_vDestPosition);

	m_pEffectMatrix = new _matrix;
	D3DXMatrixIdentity(m_pEffectMatrix);
	memcpy(&m_pEffectMatrix->m[3][0], &vPosition,  sizeof(_vec3));

	switch (eType)
	{
	case MONK_MISSILE:
		m_Templist = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Missile", m_pEffectMatrix, m_vDestPosition);
		m_pCollisionSphere->Set_Damage(20);
		break;
	case MAGE_MISSILE:
		m_Templist = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Mage_CommonAtt", m_pEffectMatrix, m_vDestPosition);
		m_pCollisionSphere->Set_Damage(30);
		break;
	case MAGE_TOWER_MISSILE:
		m_Templist =Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MissileTower_Missile", m_pEffectMatrix, m_vDestPosition);
		break;
	case MAGE_TOWER_STRIKER:
		m_Templist = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_StrikerTower_Missile", m_pEffectMatrix, m_vDestPosition);
		break;
	}	
	m_pCollisionSphere->Set_MatrixWorldCollision(m_pTransformCom->Get_WorldMatrix());
	m_pCollisionSphere->Set_CheckCollsion(false);
	return NOERROR;
}

int CMissile::Update_GameObject(const float & fTimeDelta)
{
	if (m_tInfo.bDead)
	{
		list<Engine::CEffect*>::iterator iter_begin = m_Templist.begin();
		list<Engine::CEffect*>::iterator iter_end = m_Templist.end();
		for (; iter_begin != iter_end; ++iter_begin)
			(*iter_begin)->Set_Dead();
		m_Templist.clear();
		_vec3 Pos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		
		Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_Templist);

		switch (m_eMissileType)
		{
		case MONK_MISSILE:
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MissileDel", &m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION));
			if (fDistance < 250.f)
				CSoundMgr::GetInstance()->Play_Sound(L"MagicBolt_Impact.ogg", Engine::CHANNEL_NORMAL_ATTACK, 0.5f);
			break;
		case MAGE_MISSILE:
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Mage_CommonAtt_Del", &m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION));
			if (fDistance < 250.f)
				CSoundMgr::GetInstance()->Play_Sound(L"MagicBolt_Impact2.ogg", Engine::CHANNEL_NORMAL_ATTACK, 0.5f);
			break;
		case MAGE_TOWER_MISSILE:
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MissileTower_Missile_Del", &m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION));
			if (fDistance < 250.f)
				CSoundMgr::GetInstance()->Play_Sound(L"Tower_DeadlyStrikerImpact2.ogg", Engine::CHANNEL_TOWER_ATTACK, 0.5f);
			break;
		case MAGE_TOWER_STRIKER:
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MissileTower_Missile_Del", &m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION));
			if (fDistance < 250.f)
				CSoundMgr::GetInstance()->Play_Sound(L"Tower_DeadlyStrikerImpact3.ogg", Engine::CHANNEL_TOWER_ATTACK, 0.5f);
			break;
		}
		m_pCollisionSphere->Set_CheckCollsion(false);
		return 1;
	}
	m_pCollisionSphere->Set_Damage(m_tInfo.iAttPower);

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

void CMissile::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

}

void CMissile::Check_Collision(void)
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
	if (D3DXVec3Length(&(m_vDestPosition - vPostion)) < 3.f)
		m_tInfo.bDead = true;
}

HRESULT CMissile::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*	pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Weapon", 2.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
	m_pCollisionManager->Add_Component(L"Com_Col_Weapon", m_pCollisionSphere);

	return NOERROR;
}

HRESULT CMissile::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CMissile* CMissile::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, _vec3 vDir, MISSILE_TYPE eType)
{
	CMissile* pInstance = new CMissile(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPosition, vDir, eType)))
	{
		MessageBox(0, L"CMissile Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CMissile::Free(void)
{
	m_pCollisionManager->Delete_Component(L"Com_Col_Weapon", m_pCollisionSphere);
	Engine::Safe_Delete(m_pEffectMatrix);
	CCollisionObject::Free();
}
