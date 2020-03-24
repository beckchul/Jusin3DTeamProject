#include "stdafx.h"
#include "Tower.h"
#include "CollisionManager.h"
#include "SoundMgr.h"
#include "EffectMgr.h"
#include "Font_PlayerDamage.h"
#include "Object_Manager.h"

CTower::CTower(LPDIRECT3DDEVICE9 pGraphicDev)
: CDynamicObject(pGraphicDev)
, m_pCollisionManager(CCollisionManager::GetInstance())
, m_pCollisionBody(nullptr)
, m_pTargetSphere(nullptr)
, m_vecPrevColor(1.f, 1.f, 1.f, 1.f)
, m_bSummoning(false)
, m_vecColor(0.f, 0.f, 0.f, 0.f)
, m_vecTransColor(0.f, 0.f, 0.f, 0.f)
{
	m_pCollisionManager->Add_Ref();

	m_tTargetCheck.fRength = 70.f;
	m_tTargetCheck.bTarget = false;
	m_tTargetCheck.fAngle = 90.f;

	m_fColorLimitTime = 0.1f;
}

CTower::~CTower()
{
}

void CTower::Set_TowerEffect(TOWER_TYPE eType)
{
	_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);

	switch (eType)
	{
	case TOWER_TYPE_MISSILE:
		m_pTower_BallEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MissileTower_Ball"
			, m_pDynamicMeshCom->Get_FrameMatrix("b_Spike4_3")
			, m_pTransformCom->Get_WorldMatrix());
		m_pTower_UnderEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_UnderMissileTower", &vPos);
		break;
	case TOWER_TYPE_BLOCKADE:
		break;
	case TOWER_TYPE_LIGHTNING:
		m_pTower_BallEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_LightingTower_Ball"
			, m_pDynamicMeshCom->Get_FrameMatrix("b_COG")
			, m_pTransformCom->Get_WorldMatrix());
		m_pTower_UnderEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_UnderLightingTower", &vPos);
		break;
	case TOWER_TYPE_FIRE:
		m_pTower_BallEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_FireTower_Ball"
			, m_pDynamicMeshCom->Get_FrameMatrix("b_Spike4_3")
			, m_pTransformCom->Get_WorldMatrix());
		m_pTower_UnderEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_UnderFireTower", &vPos);
		break;
	case TOWER_TYPE_STRIKER:
		m_pTower_BallEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_StrikerTower_Ball"
			, m_pDynamicMeshCom->Get_FrameMatrix("b_COG")
			, m_pTransformCom->Get_WorldMatrix());
		m_pTower_UnderEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_UnderStrikerTower", &vPos);
		break;
	}
}

void CTower::Appear(const float& fTimeDelta, const float& fScale)
{
	if (m_pTransformCom->Get_Info(CTransform::INFO_SCALE).x >= fScale)
	{
		m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(fScale, fScale, fScale));
		return;
	}
		
	_vec3 vecScale = { fTimeDelta * 0.25f, fTimeDelta * 0.25f, fTimeDelta * 0.25f };
	m_pTransformCom->Change_Information(CTransform::INFO_SCALE, &vecScale);
}

void CTower::Check_Collision(void)
{
	auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonWeapon");
	for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
	{
		if (m_pCollisionBody->Check_Collision((CCollisionSphere*)(*iter)) && !m_pCollisionBody->Get_HitCheck())
		{
			m_tInfo.iHp -= ((CCollisionSphere*)(*iter))->Get_Damage();
			
			// Hit됐을때 컬러값 변경.
			m_bColorAdd = true;
			m_fColorTime = 0.f;

			// 데미지 폰트
			Engine::CGameObject* pGameObject = CFont_PlayerDamage::Create(m_pGraphicDev, ((CCollisionSphere*)(*iter))->Get_Damage());
			pGameObject->Set_Pos(&(m_pTransformCom->Get_Info(CTransform::INFO_POSITION) + _vec3(0.f, 28.f, 0.f)));
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject);

			_vec3 Pos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
			_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
			float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));

			if (0 >= m_tInfo.iHp)
			{
				m_tInfo.bDead = true;
				
				m_pCollisionBody->Set_HitCheck(true);
				if (fDistance < 300.f)
				{
					CSoundMgr::GetInstance()->Play_Sound(L"TowerDestroy2.ogg", Engine::CHANNEL_TOWER_3, 0.8f);
				}
			}
			else
			{
				m_pCollisionBody->Set_HitCheck(true);
				if (fDistance < 300.f && FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_TOWER_3))
					CSoundMgr::GetInstance()->Play_RandomSound(L"TowerDamage.ogg", Engine::CHANNEL_TOWER_3, 4, 0.8f);
			}
		}
	}
}

void CTower::Check_Target(void)
{
	if (m_vecPrevColor != _vec4(1.f, 1.f, 1.f, 1.f))
		return;

	if (NULL == m_pTargetSphere || m_pTargetSphere->Get_Life() == -1.f)
	{
		m_tTargetCheck.bTarget = false;
		Check_TargetAll();
		return;
	}
	if(!m_pTargetSphere->Get_CheckCollision())
	{
		m_pTargetSphere = nullptr;
		m_tTargetCheck.bTarget = false;
		return;
	}

	_matrix matDest = m_pTargetSphere->Get_WorldMatrix();
	_matrix matSour = *(m_pTransformCom->Get_WorldMatrix());

	_vec3 vecDestPosition, vecSourPosition;
	memcpy_s(&vecDestPosition, sizeof(_vec3), &(matDest.m[3][0]), sizeof(_vec3));
	memcpy_s(&vecSourPosition, sizeof(_vec3), &(matSour.m[3][0]), sizeof(_vec3));

	_vec3 vecTargetDir = (vecDestPosition - vecSourPosition);

	if (m_tTargetCheck.fRength < D3DXVec3Length(&vecTargetDir))
	{
		m_pTargetSphere = nullptr;
		m_tTargetCheck.bTarget = false;
		return;
	}

	_vec3 vecDir;
	memcpy(&vecDir, &matSour.m[2][0], sizeof(_vec3));

	D3DXVec3Normalize(&vecDir, &vecDir);
	D3DXVec3Normalize(&vecTargetDir, &vecTargetDir);

	float fTargetAngle = D3DXVec3Dot(&vecDir, &vecTargetDir);

	if (D3DXToDegree(acosf(fTargetAngle)) > m_tTargetCheck.fAngle / 2.f)
	{
		m_pTargetSphere = nullptr;
		m_tTargetCheck.bTarget = false;
		return;
	}
}

void CTower::Check_TargetAll(void)
{
	float fLegth;
	float fMinLegth = 100.f;

	auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonBody");
	for (auto iter : pCollisionlist)
	{
		if(!((CCollisionSphere*)iter)->Get_CheckCollision())
			continue;

		_matrix matDest = ((CCollisionSphere*)iter)->Get_WorldMatrix();
		_matrix matSour = *(m_pTransformCom->Get_WorldMatrix());

		_vec3 vecDestPosition, vecSourPosition;
		memcpy_s(&vecDestPosition, sizeof(_vec3), &(matDest.m[3][0]), sizeof(_vec3));
		memcpy_s(&vecSourPosition, sizeof(_vec3), &(matSour.m[3][0]), sizeof(_vec3));

		_vec3 vecTargetDir = (vecDestPosition - vecSourPosition);

		if(m_tTargetCheck.fRength < D3DXVec3Length(&vecTargetDir))
			continue;
		fLegth = D3DXVec3Length(&vecTargetDir);

		_vec3 vecDir;
		memcpy(&vecDir, &matSour.m[2][0], sizeof(_vec3));
		
		D3DXVec3Normalize(&vecDir, &vecDir);
		D3DXVec3Normalize(&vecTargetDir, &vecTargetDir);

		float fTargetAngle = D3DXVec3Dot(&vecDir, &vecTargetDir);
		
		if(D3DXToDegree(acosf(fTargetAngle)) > m_tTargetCheck.fAngle / 2.f)
			continue;

		if (fMinLegth > fLegth)
		{
			fMinLegth = fLegth;
			m_pTargetSphere = ((CCollisionSphere*)iter);
			m_tTargetCheck.bTarget = true;
			return;
		}
	}
}


void CTower::Play_Animation(const float & fTimeDelta)
{

	if (m_iPreRenderState != m_iRenderState)
	{
		m_iPreRenderState = m_iRenderState;
		m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[m_iRenderState]);
	}


	m_fCount += fTimeDelta * 2.f;
	if (m_fCount >= 1.f)
	{
		m_fCount = 0.f;
		++m_fAttackDelay;
	}
	if (nullptr != m_pCollisionBody)
	{
		if (true == m_pCollisionBody->Get_HitCheck())
		{
			m_fHitTime += fTimeDelta * 2.f;
			if (m_fHitTime >= 1.f)
			{
				m_pCollisionBody->Set_HitCheck(false);
				m_fHitTime = 0.f;	
			}
		}	
	}
	
	if (m_pDynamicMeshCom->GetPeriod() - m_vecAnimationInfo[m_iRenderState].dEndAniCount <= m_pDynamicMeshCom->GetTrackPosition())
	{
		if (m_eMainState == TOWER_FIRE)
			m_fAttackDelay = 0;
		m_eMainState = TOWER_IDLE;
		m_iPreRenderState = -1;
		m_iAttackCount = 0;
	}

	if (!m_tInfo.bDead)
		m_pDynamicMeshCom->Play_AnimationSet(fTimeDelta);
}

void CTower::Change_MainState(void)
{
	if (m_tTargetCheck.bTarget && m_fAttackDelayCount <= m_fAttackDelay)
	{
		m_eMainState = TOWER_FIRE;
	}
	else
		m_eMainState = TOWER_IDLE;
}


void CTower::Free(void)
{
	Engine::Safe_Release(m_pCollisionBody);
	Engine::Safe_Release(m_pCollisionManager);
	CDynamicObject::Free();
}
