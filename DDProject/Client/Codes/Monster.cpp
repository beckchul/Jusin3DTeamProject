#include "stdafx.h"
#include "Monster.h"
#include "AnimationCtrl.h"
#include "PlayerObserver.h"
#include "CollisionSphere.h"
#include "CollisionManager.h"
#include "NavMgr.h"
#include "DataManager.h"
#include "Font_Damage.h"
#include "Object_Manager.h"
#include "SoundMgr.h"
#include "Present.h"

#include "Effect_Particle.h"
#include "EffectMgr.h"

#include "InfoSubject.h"
#include "WaveObserver.h"

#include "SplashDamage.h"

#include "InputDev.h"
USING(Engine)

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
: CDynamicObject(pGraphicDev)
, m_pPlayerObserver(nullptr)
, m_eMainState(MONSTER_IDLE)
, m_iRenderState(0)
, m_iPreRenderState(0)
, m_pCollisionBody(nullptr)
, m_pCollsionWeapon(nullptr)
, m_pCollisionManager(CCollisionManager::GetInstance())
, m_dwIndex(-1)
, m_bFirstFrame(false)
, m_bHurt(true)
, m_pNavMgr(Engine::CNavMgr::GetInstance())
, m_pInfoSubject(Engine::CInfoSubject::GetInstance())
, m_eMonsterType(MONSTER_TYPE_END)
, m_vecColor(0.f, 0.f, 0.f, 0.f)
, m_vecTransColor(0.f, 0.f, 0.f, 0.f)
{
	m_pCollisionManager->Add_Ref();
	m_pNavMgr->Add_Ref();
	m_pInfoSubject->Add_Ref();

	m_tInfo.fSpeedX = 10.f;

	m_tFllowTarget.bTargetCheck = false;
	m_tFllowTarget.bTargetFllow = false;
	m_tFllowTarget.fTargetLength = 0.f;
	m_tFllowTarget.fAttackRange = 25.f;
	m_tFllowTarget.fSearchLength = 100.f;
	m_tFllowTarget.eMonsterTurn = MONSTER_TURN_END;

	m_fColorLimitTime = 0.1f;
}

CMonster::~CMonster(void)
{
}

void CMonster::Monster_Fall(const float & fTimeDelta)
{
	m_tInfo.fSpeedY -= 5.f * fTimeDelta;

	m_pTransformCom->Go_Down(m_tInfo.fSpeedY);

	bool bDropKill = false;
	if (TRUE == m_pNavMgr->FallOnNavMesh(m_pTransformCom, m_dwIndex, bDropKill)) // Collide with NavMesh
		m_tInfo.fSpeedY = 0.f;

	if (bDropKill) // 낙사
		m_eMainState = MONSTER_DEAD;

	if (m_pTransformCom->Get_Info(CTransform::INFO_POSITION).y <= -300.f)
	{
		m_eMainState = MONSTER_DEAD;
	}
}

void CMonster::Create_Route(Engine::OBJECTID eRoute)
{
	m_vecMonsterRoute = CDataManager::GetInstance()->Get_MonsterRoute(eRoute);
}

_bool CMonster::Search_Target(const float & fTimeDelta)
{
	if (nullptr == m_pPlayerObserver)
		return false;
	
	if ((MONSTER_DEAD == m_eMainState ||
		MONSTER_HURT == m_eMainState ||
		MONSTER_ATTACK == m_eMainState ||
		MONSTER_ATTACK_2 == m_eMainState))
	{
		if (m_bFuse)
			return true;
		else
			return false;
	}
		
	_bool bCheck = false;
	_float fLegth;
	_float fMinLegth = 80.f;
	_vec3 vecMinTargetDir;
	_matrix matMinSourMatrix; 

	if (m_eHurtState == MONSTER_ENRAGE)
	{
		auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonBody");		// Enrage Monster
		for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
		{
			if((*iter) == m_pCollisionBody)
				continue;
			_matrix* pDestMatrix = &((CCollisionSphere*)(*iter))->Get_WorldMatrix();
			_matrix* pSourMatrix = m_pTransformCom->Get_WorldMatrix();

			_vec3 vecDestPosition, vecSourPosition;
			memcpy_s(&vecDestPosition, sizeof(_vec3), &(pDestMatrix->m[3][0]), sizeof(_vec3));
			memcpy_s(&vecSourPosition, sizeof(_vec3), &(pSourMatrix->m[3][0]), sizeof(_vec3));

			_vec3 vecTargetDir = (vecDestPosition - vecSourPosition);

			fLegth = D3DXVec3Length(&vecTargetDir);
			if (fMinLegth > fLegth)
			{
				fMinLegth = fLegth;
				vecMinTargetDir = vecTargetDir;
				matMinSourMatrix = (*pSourMatrix);
				bCheck = true;
			}
		}
	}
	if (!bCheck)
	{
		auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_Crystal");	// 크리스탈
		for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
		{
			_matrix* pDestMatrix = &((CCollisionSphere*)(*iter))->Get_WorldMatrix();
			_matrix* pSourMatrix = m_pTransformCom->Get_WorldMatrix();

			_vec3 vecDestPosition, vecSourPosition;
			memcpy_s(&vecDestPosition, sizeof(_vec3), &(pDestMatrix->m[3][0]), sizeof(_vec3));
			memcpy_s(&vecSourPosition, sizeof(_vec3), &(pSourMatrix->m[3][0]), sizeof(_vec3));

			_vec3 vecTargetDir = (vecDestPosition - vecSourPosition);

			fLegth = D3DXVec3Length(&vecTargetDir);
			if (fMinLegth > fLegth)
			{
				fMinLegth = fLegth;
				vecMinTargetDir = vecTargetDir;
				matMinSourMatrix = (*pSourMatrix);
				bCheck = true;
			}
		}
	}

	if (!bCheck)
	{
		auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_Tower");		// 타워
		for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
		{
			_matrix* pDestMatrix = &((CCollisionSphere*)(*iter))->Get_WorldMatrix();
			_matrix* pSourMatrix = m_pTransformCom->Get_WorldMatrix();

			_vec3 vecDestPosition, vecSourPosition;
			memcpy_s(&vecDestPosition, sizeof(_vec3), &(pDestMatrix->m[3][0]), sizeof(_vec3));
			memcpy_s(&vecSourPosition, sizeof(_vec3), &(pSourMatrix->m[3][0]), sizeof(_vec3));

			_vec3 vecTargetDir = (vecDestPosition - vecSourPosition);

			fLegth = D3DXVec3Length(&vecTargetDir);
			if (fMinLegth > fLegth)
			{
				fMinLegth = fLegth;
				vecMinTargetDir = vecTargetDir;
				matMinSourMatrix = (*pSourMatrix);
				bCheck = true;
			}
		}
	}

	if (!bCheck)
	{
		for (int i = PLAYER_1; i < PLAYER_INDEX_END; ++i)				// 플레이어
		{
			const PLAYER_INFO* pPlayerInfo = m_pPlayerObserver->Get_PlayerInfo(i);

			if (nullptr == pPlayerInfo || pPlayerInfo->eMainState == PLAYER_DEAD)
				continue;

			_matrix* pDestMatrix = pPlayerInfo->pTransform->Get_WorldMatrix();
			_matrix* pSourMatrix = m_pTransformCom->Get_WorldMatrix();

			_vec3 vecDestPosition, vecSourPosition;
			memcpy_s(&vecDestPosition, sizeof(_vec3), &(pDestMatrix->m[3][0]), sizeof(_vec3));
			memcpy_s(&vecSourPosition, sizeof(_vec3), &(pSourMatrix->m[3][0]), sizeof(_vec3));

			_vec3 vecTargetDir = (vecDestPosition - vecSourPosition);

			fLegth = D3DXVec3Length(&vecTargetDir);
			if (fMinLegth > fLegth)
			{
				fMinLegth = fLegth;
				vecMinTargetDir = vecTargetDir;
				matMinSourMatrix = (*pSourMatrix);
				bCheck = true;
			}
		}
	}

	if (!bCheck)
	{
		m_tFllowTarget.bTargetCheck = false;
		m_tFllowTarget.bTargetFllow = false;
		m_tFllowTarget.eMonsterTurn = MONSTER_TURN_END;
		m_eMainState = MONSTER_IDLE;
		return false;
	}
	else if ((m_tFllowTarget.fTargetLength = D3DXVec3Length(&vecMinTargetDir)) <= m_tFllowTarget.fSearchLength)
		m_tFllowTarget.bTargetCheck = true;
	else
	{
		m_tFllowTarget.bTargetCheck = false;
		m_tFllowTarget.bTargetFllow = false; 
		m_tFllowTarget.eMonsterTurn = MONSTER_TURN_END;
		m_eMainState = MONSTER_IDLE;
	}

	if (!m_tFllowTarget.bTargetCheck)
		return false;

	if (MONSTER_TURN_FINISH != m_tFllowTarget.eMonsterTurn)
	{
		// 기본 방향 회전
		_vec3 vInfoDir;
		memcpy_s(&vInfoDir, sizeof(_vec3), &(matMinSourMatrix.m[2][0]), sizeof(_vec3));
		D3DXVec3Normalize(&vInfoDir, &vInfoDir);

		_vec3 vLeftDir;
		D3DXVec3Cross(&vLeftDir, &vInfoDir, &_vec3(0.0f, 1.0f, 0.0f));
		D3DXVec3Normalize(&vecMinTargetDir, &vecMinTargetDir);

		m_eMainState = MONSTER_TURN;

		if (D3DXVec3Dot(&vLeftDir, &vecMinTargetDir) < 0)
		{
			//Left
			m_pTransformCom->Rotation(CTransform::ANGLE_Y, fTimeDelta * D3DXToRadian(180.f));
			if(MONSTER_TURN_END == m_tFllowTarget.eMonsterTurn)
				m_tFllowTarget.eMonsterTurn = MONSTER_TURN_LEFT;
			else if(MONSTER_TURN_RIGHT == m_tFllowTarget.eMonsterTurn)
				m_tFllowTarget.eMonsterTurn = MONSTER_TURN_FINISH;
		}
		else
		{
			//Right
			m_pTransformCom->Rotation(CTransform::ANGLE_Y, fTimeDelta * D3DXToRadian(-180.f));
			if (MONSTER_TURN_END == m_tFllowTarget.eMonsterTurn)
				m_tFllowTarget.eMonsterTurn = MONSTER_TURN_RIGHT;
			else if (MONSTER_TURN_LEFT == m_tFllowTarget.eMonsterTurn)
				m_tFllowTarget.eMonsterTurn = MONSTER_TURN_FINISH;
		}
	}
	else
	{
		// 회전 이동 후
		float fAngle = atan2f(vecMinTargetDir.z, vecMinTargetDir.x);

		m_pTransformCom->SetRotation(CTransform::ANGLE_Y, -fAngle + D3DXToRadian(90.f));
		m_tFllowTarget.bTargetFllow = true;
		m_tFllowTarget.vTargetDir = vecMinTargetDir;
	}

	return true;
}

void CMonster::Follow_Player(const float& fTimeDelta)
{
	if (!m_tFllowTarget.bTargetFllow)
		return;

	if (MONSTER_HURT == m_eMainState || MONSTER_DEAD == m_eMainState)
		return;
	

	if (m_tFllowTarget.fTargetLength > m_tFllowTarget.fAttackRange)
	{
		
		if (m_eMonsterType == MONSTER_KOBOLD)
		{
			if (!m_bFuse)
				m_eMainState = MONSTER_ATTACK;
			else
			{
				m_eMainState = MONSTER_ATTACK_2;

				float fAngle = atan2f(m_tFllowTarget.vTargetDir.z, m_tFllowTarget.vTargetDir.x);
				m_pTransformCom->SetRotation(CTransform::ANGLE_Y, -fAngle + D3DXToRadian(90.f));
				m_dwIndex = m_pTransformCom->Go_Straight(fTimeDelta * m_tInfo.fSpeedX * m_fSlowSpeed * 2.f, m_dwIndex);
				m_tFllowTarget.bTargetFllow = true;
			}
		}
		else
		{
			m_dwIndex = m_pTransformCom->Go_Straight(fTimeDelta * m_tInfo.fSpeedX * m_fSlowSpeed, m_dwIndex);

			m_eMainState = MONSTER_MOVE;
		}
	}
	else if (MONSTER_HURT != m_eMainState)
	{
		if (m_bFuse)
		{
			m_tInfo.bDead = true;
			m_bFuse = false;
			m_fDeadTime = 100.f;
			Engine::CGameObject*		pGameObject = nullptr;
			pGameObject = CSplashDamage::Create(m_pGraphicDev, m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION), KOBOLD_SPLASH_ATTACK);
			((CSplashDamage*)pGameObject)->Set_Damage(m_tInfo.iAttPower);
			if (FAILED(Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
				return;
			_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
			float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
			if (fDistance < 300.f)
			{
				CSoundMgr::GetInstance()->StopSound(CHANNEL_KOBOLD);
				CSoundMgr::GetInstance()->Play_RandomSound(L"Kobold_explode.ogg", CHANNEL_KOBOLD, 2, 0.8f);
			}
		}
	
		if (rand() % 2 == 0)
		{
			m_tFllowTarget.bTargetFllow = false;
			m_eMainState = MONSTER_ATTACK;
		}
		else
		{
			m_tFllowTarget.bTargetFllow = false;
			m_eMainState = MONSTER_ATTACK_2;
		}
	}
}

void CMonster::Work_Route(const float & fTimeDelta)
{	
	if (MONSTER_DEAD == m_eMainState ||
		MONSTER_HURT == m_eMainState ||
		MONSTER_ATTACK == m_eMainState ||
		MONSTER_ATTACK_2 == m_eMainState)
		return;

	_matrix matWorld = *m_pTransformCom->Get_WorldMatrix();
	_vec3 vPos;
	memcpy(&vPos, &matWorld.m[3][0], sizeof(_vec3));

	_vec3 vRote = m_vecMonsterRoute.front();

	_vec3 vDir, vLength = vRote - vPos;

	D3DXVec3Normalize(&vDir, &vLength);
	float fAngle = atan2f(vDir.z, vDir.x);

	m_pTransformCom->SetRotation(CTransform::ANGLE_Y, -fAngle + D3DXToRadian(90.f));
	m_eMainState = MONSTER_MOVE;
	m_dwIndex = m_pTransformCom->Go_Straight(fTimeDelta * m_tInfo.fSpeedX * m_fSlowSpeed, m_dwIndex);

	
	if (D3DXVec3Length(&vLength) < 10.f)
	{
		if (m_vecMonsterRoute.size())
		{
			auto iter = m_vecMonsterRoute.begin();
			iter = m_vecMonsterRoute.erase(iter);
		}
	}
}

void CMonster::Play_Animation(const float& fTimeDelta)
{
	if (m_tInfo.bDead)
		return;

	if (m_iPreRenderState != m_iRenderState)
	{
		m_iAttackState = 0;

		m_iPreRenderState = m_iRenderState;
		if (!m_tInfo.bDead)
			m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[m_iRenderState]);
	}
	
	if (m_pDynamicMeshCom->GetPeriod() - m_vecAnimationInfo[m_iRenderState].dEndAniCount <= m_pDynamicMeshCom->GetTrackPosition())
	{

		if (m_eMonsterType == MONSTER_KOBOLD && m_eMainState == MONSTER_ATTACK)
		{
			m_bFuse = true;
		}
		
		switch (m_eMainState)
		{
		case MONSTER_ATTACK_4:
		case MONSTER_MOVE_LEFT:
		case MONSTER_MOVE_RIGHT:
			m_iPreRenderState = -1;
			break;
		case MONSTER_DEAD:
			m_tInfo.bDead = true;
			m_fDeadTrackPosition = (_float)m_pDynamicMeshCom->GetPeriod();
			m_iDeadTrackIndex = m_iRenderState;
			m_pCollisionBody->Set_CheckCollsion(false);
			return;
		case MONSTER_FLY:
			m_bFly = true;
			m_eMainState = MONSTER_IDLE;
			m_iPreRenderState = -1;
			break;
		case MONSTER_CINEMATIC_LANDING:
			m_eMainState = MONSTER_CINEMATIC_END;
			m_iPreRenderState = -1;
			break;
		default:
			m_iPreRenderState = -1;
			if (m_eMainState == MONSTER_MOVE && m_eMonsterType == MONSTER_DEMON)
				break;
			else if (m_eMainState == MONSTER_BACK_MOVE && m_eMonsterType == MONSTER_DEMON)
				break;
			m_eMainState = MONSTER_IDLE;
		}

		
	}
	m_pDynamicMeshCom->Play_AnimationSet(fTimeDelta * m_fSlowSpeed);
}

void CMonster::Check_Collision(const float& fTimeDelta)
{
	if (!m_bFirstFrame)
		return;

	if (CInputDev::GetInstance()->StayKeyDown(DIK_L))
	{
		m_eMainState = MONSTER_DEAD;
	}

	if (m_eMainState == MONSTER_DEAD)
		return;

	if (nullptr != m_pCollisionBody)
	{
		if (true == m_pCollisionBody->Get_HitCheck())
		{
			m_fHitTime += fTimeDelta * 3.f;
			if (m_fHitTime >= 1.f)
			{
				m_pCollisionBody->Set_HitCheck(false);
				m_fHitTime = 0.f;
			}
		}
	}
	// 데미지 체크
	auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_Weapon");							// Damage
	for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
	{
		if (m_pCollisionBody->Check_Collision((CCollisionSphere*)(*iter)) && !m_pCollisionBody->Get_HitCheck())
		{
			_uint uDamage = ((CCollisionSphere*)(*iter))->Get_Damage();
			m_tInfo.iHp -= uDamage;
			if (0 >= m_tInfo.iHp)
			{
				m_tInfo.iHp = 0;
				m_eMainState = MONSTER_DEAD;
				m_bFly = false;
				// Effect--
				_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
				_vec3 vColPos;
				memcpy(&vColPos, &((CCollisionSphere*)(*iter))->Get_WorldMatrix().m[3][0], sizeof(_vec3));
				m_vDeadDir = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION) - vColPos;
				D3DXVec3Normalize(&m_vDeadDir, &m_vDeadDir);
				m_tInfo.fSpeedY = 2.f;

				switch (m_eMonsterType)
				{
				case MONSTER_GOBLIN:
					Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterDeadGreenBlood", m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), m_pTransformCom->Get_WorldMatrix());
					break;
				case MONSTER_DARKELF:
					Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterDeadPinkBlood", m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), m_pTransformCom->Get_WorldMatrix());
					break;
				}
				
				// Sound
				_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
				float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
				if (fDistance < 300.f)
				{
					switch (m_eMonsterType)
					{
					case MONSTER_GOBLIN:
						//if(FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_GOLBIN))
							CSoundMgr::GetInstance()->Play_RandomSound(L"Goblin_death.ogg", Engine::CHANNEL_GOLBIN, 6, 0.8f);
						break;
					case MONSTER_DARKELF:
					//	if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_DARKELF))
							CSoundMgr::GetInstance()->Play_RandomSound(L"DarkElf_Death.ogg", Engine::CHANNEL_DARKELF, 2, 0.8f);
						break;
					case MONSTER_ORC:
					//	if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_ORC))
							CSoundMgr::GetInstance()->Play_RandomSound(L"Orc_death.ogg", Engine::CHANNEL_ORC, 2, 0.8f);
						break;
					case MONSTER_KOBOLD:
						CSoundMgr::GetInstance()->Play_RandomSound(L"Kobold_death.ogg", Engine::CHANNEL_KOBOLD, 2, 0.8f);
						break;
					case MONSTER_DEMON:
						CSoundMgr::GetInstance()->Play_Sound(L"cromcruach_threat_02.wav", Engine::CHANNEL_DEMON, 0.8f);
						break;
					}
				}
			}
			else if (m_bHurt)
			{
				if(m_bFuse)
					continue;

				if(m_eMonsterType != MONSTER_DEMON)
					m_eMainState = MONSTER_HURT;

				int iRand = Random(3);
				_matrix* matWorld = m_pTransformCom->Get_WorldMatrix();
				if (m_eMonsterType == MONSTER_DARKELF)	// 다크엘프는 분홍 피 튀김 
				{
					switch (iRand)
					{
					case 0:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterPinkBlood1", matWorld);
						break;
					case 1:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterPinkBlood2", matWorld);
						break;
					case 2:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterPinkBlood3", matWorld);
						break;
					}
					_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
					Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterPinkExplosion", &_vec3(vPos.x, vPos.y + 14.f, vPos.z));

				}
				else if (m_eMonsterType == MONSTER_DEMON)
				{
					switch (iRand)
					{
					case 0:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterRedBlood1", matWorld);
						break;
					case 1:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterRedBlood2", matWorld);
						break;
					case 2:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterRedBlood3", matWorld);
						break;
					}
					Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterRedExplosion", matWorld);
				}
				else // 나머지 초록피 튀김 
				{
					switch (iRand)
					{
					case 0:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenBlood1", matWorld);
						break;
					case 1:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenBlood2", matWorld);
						break;
					case 2:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenBlood3", matWorld);
						break;
					}
					_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
					Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenExplosion", &_vec3(vPos.x, vPos.y + 14.f, vPos.z));
				}


				// Sound
				_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
				float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
				if (fDistance < 300.f)
				{
					switch (m_eMonsterType)
					{
					case MONSTER_GOBLIN:
						if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_GOLBIN))
							CSoundMgr::GetInstance()->Play_RandomSound(L"Goblin_hurt.ogg", Engine::CHANNEL_GOLBIN, 3, 0.6f);
						break;
					case MONSTER_DARKELF:
						if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_DARKELF))
							CSoundMgr::GetInstance()->Play_RandomSound(L"DarkElf_Hurt.ogg", Engine::CHANNEL_DARKELF, 4, 0.6f);
						break;
					case MONSTER_ORC:
						if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_ORC))
							CSoundMgr::GetInstance()->Play_RandomSound(L"Orc_hurt.ogg", Engine::CHANNEL_ORC, 3, 0.6f);
						break;
					case MONSTER_KOBOLD:
						if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_KOBOLD))
							CSoundMgr::GetInstance()->Play_RandomSound(L"Kobold_hurt.ogg", Engine::CHANNEL_KOBOLD, 3, 0.8f);
						break;
					}
				}

			}
			m_pCollisionBody->Set_HitCheck(true);

			// 데미지 폰트
			Engine::CGameObject* pGameObject = CFont_Damage::Create(m_pGraphicDev, uDamage);
			pGameObject->Set_Pos(& (m_pTransformCom->Get_Info(CTransform::INFO_POSITION) + _vec3(0.f, 30.f, 0.f)));
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject);


			// Hit됐을때 컬러값 변경.
			m_bColorAdd = true;
			m_fColorTime = 0.f;

			m_pCollisionBody->Set_HitCheck(true);
		}
	}

	m_fSlowSpeed = 1.f;
	auto pCollisionlist2 = m_pCollisionManager->Find_Componentlist(L"Com_Col_SlowAura");				// Slow Aura
	for (auto iter = pCollisionlist2.begin(); iter != pCollisionlist2.end(); ++iter)
	{
		if (m_pCollisionBody->Check_AuraCollision((CCollisionSphere*)(*iter)))
		{
			m_fSlowSpeed = ((CCollisionSphere*)(*iter))->Get_Damage() * 0.1f;
			((CCollisionSphere*)(*iter))->Set_Life(1.f);
			break;
		}
	}

	auto pCollisionlist3 = m_pCollisionManager->Find_Componentlist(L"Com_Col_LightningAura");			// Lightning Aura
	for (auto iter = pCollisionlist3.begin(); iter != pCollisionlist3.end(); ++iter)
	{
		if (m_pCollisionBody->Check_AuraCollision((CCollisionSphere*)(*iter)) && !m_pCollisionBody->Get_HitCheck())
		{
			_uint uDamage = ((CCollisionSphere*)(*iter))->Get_Damage();
			((CCollisionSphere*)(*iter))->Set_Life(1.f);
			m_tInfo.iHp -= uDamage;
			if (0 >= m_tInfo.iHp)
			{
				m_tInfo.iHp = 0;
				m_eMainState = MONSTER_DEAD;
				m_bFly = false;
				// Effect--
				_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
				_vec3 vColPos;
				memcpy(&vColPos, &((CCollisionSphere*)(*iter))->Get_WorldMatrix().m[3][0], sizeof(_vec3));
				m_vDeadDir = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION) - vColPos;
				D3DXVec3Normalize(&m_vDeadDir, &m_vDeadDir);
				m_tInfo.fSpeedY = 2.f;

				Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Monster_Dead", &_vec3(vPos.x, vPos.y + 10.f, vPos.z));
				
				// Sound
				_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
				float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
				if (fDistance < 300.f)
				{
					switch (m_eMonsterType)
					{
					case MONSTER_GOBLIN:
						//if(FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_GOLBIN))
						CSoundMgr::GetInstance()->Play_RandomSound(L"Goblin_death.ogg", Engine::CHANNEL_GOLBIN, 6, 0.8f);
						break;
					case MONSTER_DARKELF:
						//	if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_DARKELF))
						CSoundMgr::GetInstance()->Play_RandomSound(L"DarkElf_Death.ogg", Engine::CHANNEL_DARKELF, 2, 0.8f);
						break;
					case MONSTER_ORC:
						//	if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_ORC))
						CSoundMgr::GetInstance()->Play_RandomSound(L"Orc_death.ogg", Engine::CHANNEL_ORC, 2, 0.8f);
						break;
					case MONSTER_KOBOLD:
						CSoundMgr::GetInstance()->Play_RandomSound(L"Kobold_death.ogg", Engine::CHANNEL_KOBOLD, 2, 0.8f);
						break;
					}
				}
			}
			else if (m_bHurt)
			{
				// Effect--
				int iRand = Random(3);
				_matrix* matWorld = m_pTransformCom->Get_WorldMatrix();
				if (m_eMonsterType == MONSTER_DARKELF)	// 다크엘프는 분홍 피 튀김 
				{
					switch (iRand)
					{
					case 0:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterPinkBlood1", matWorld);
						break;
					case 1:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterPinkBlood2", matWorld);
						break;
					case 2:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterPinkBlood3", matWorld);
						break;
					}
					_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
					Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterPinkExplosion", &_vec3(vPos.x, vPos.y + 14.f, vPos.z));

				}
				else // 나머지 초록피 튀김 
				{
					switch (iRand)
					{
					case 0:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenBlood1", matWorld);
						break;
					case 1:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenBlood2", matWorld);
						break;
					case 2:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenBlood3", matWorld);
						break;
					}
					_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
					Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenExplosion", &_vec3(vPos.x, vPos.y + 14.f, vPos.z));
				}
			}
			m_pCollisionBody->Set_HitCheck(true);

			// 데미지 폰트
			Engine::CGameObject* pGameObject = CFont_Damage::Create(m_pGraphicDev, uDamage);
			pGameObject->Set_Pos(&(m_pTransformCom->Get_Info(CTransform::INFO_POSITION) + _vec3(0.f, 30.f, 0.f)));
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject);
		}
	}

	m_fLowDamage = 1.f;
	auto pCollisionlist4 = m_pCollisionManager->Find_Componentlist(L"Com_Col_DrainAura");		// Drain Aura
	for (auto iter = pCollisionlist4.begin(); iter != pCollisionlist4.end(); ++iter)
	{
		if (m_pCollisionBody->Check_AuraCollision((CCollisionSphere*)(*iter)))
		{
			m_fLowDamage = ((CCollisionSphere*)(*iter))->Get_Damage() * 0.1f;
			((CCollisionSphere*)(*iter))->Set_Life(1.f);
			break;
		}
	}

	m_eHurtState = MONSTER_NORMAL;
	auto pCollisionlist5 = m_pCollisionManager->Find_Componentlist(L"Com_Col_EnrageAura");		// Enrage Aura
	for (auto iter = pCollisionlist5.begin(); iter != pCollisionlist5.end(); ++iter)
	{
		if (m_pCollisionBody->Check_AuraCollision((CCollisionSphere*)(*iter)))
		{
			m_eHurtState = MONSTER_ENRAGE;
			((CCollisionSphere*)(*iter))->Set_Life(1.f);
			break;
		}
	}

	if (m_eHurtState == MONSTER_ENRAGE)
	{
		auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonWeapon");		// Enrage Aura
		for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
		{
			if((*iter) == m_pCollsionWeapon)
				continue;

			if (m_pCollisionBody->Check_Collision((CCollisionSphere*)(*iter)) && !m_pCollisionBody->Get_HitCheck())
			{
				_uint uDamage = ((CCollisionSphere*)(*iter))->Get_Damage();
				m_tInfo.iHp -= uDamage;
				if (0 >= m_tInfo.iHp)
				{
					m_eMainState = MONSTER_DEAD;
					m_bFly = false;
					// Effect--
					_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
					//Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenBlood_Particle", &_vec3(vPos.x, vPos.y + 15.f, vPos.z));

					// Sound
					_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
					float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
					if (fDistance < 300.f)
					{
						switch (m_eMonsterType)
						{
						case MONSTER_GOBLIN:
							if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_GOLBIN))
								CSoundMgr::GetInstance()->Play_Sound(L"Goblin_death1_2.ogg", Engine::CHANNEL_GOLBIN);
							break;
						case MONSTER_DARKELF:
							if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_DARKELF))
								CSoundMgr::GetInstance()->Play_Sound(L"DarkElfMage_death2b_2.ogg", Engine::CHANNEL_DARKELF);
							break;
						case MONSTER_ORC:
							if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_ORC))
								CSoundMgr::GetInstance()->Play_Sound(L"Orc_death2_2.ogg", Engine::CHANNEL_ORC);
							break;
						}
					}
				}
				else if (m_bHurt)
				{
					
					if (m_bFuse)
						continue; 
					if(m_eMonsterType != MONSTER_DEMON)
						m_eMainState = MONSTER_HURT;

					// Effect--
					int iRand = Random(3);
					_matrix* matWorld = m_pTransformCom->Get_WorldMatrix();
					switch (iRand)
					{
					case 0:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenBlood1", matWorld);
						break;
					case 1:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenBlood2", matWorld);
						break;
					case 2:
						Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterGreenBlood3", matWorld);
						break;
					}
				}

				//cout << "Check" << endl;
				m_pCollisionBody->Set_HitCheck(true);

				// 데미지 폰트
				Engine::CGameObject* pGameObject = CFont_Damage::Create(m_pGraphicDev, uDamage);
				pGameObject->Set_Pos(&(m_pTransformCom->Get_Info(CTransform::INFO_POSITION) + _vec3(0.f, 30.f, 0.f)));
				CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject);

				// 사운드
				_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
				float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
				if (fDistance < 300.f)
				{
					switch (m_eMonsterType)
					{
					case MONSTER_GOBLIN:
						if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_GOLBIN))
							CSoundMgr::GetInstance()->Play_Sound(L"Goblin_hurt1_2.ogg", Engine::CHANNEL_GOLBIN);
						break;
					case MONSTER_DARKELF:
						if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_DARKELF))
							CSoundMgr::GetInstance()->Play_Sound(L"DarkElfWarrior_hurt_2.ogg", Engine::CHANNEL_DARKELF);
						break;
					case MONSTER_ORC:
						if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_ORC))
							CSoundMgr::GetInstance()->Play_Sound(L"Orc_hurt1_2.ogg", Engine::CHANNEL_ORC);
						break;
					}
				}
			}
		}
	}
	return;

	// 밀쳐내기 체크 -> 프레임 드랍 심함 & 슬로우 상황에서 답이 없음
	auto pCollisionlist0 = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonBody");
	for (auto iter = pCollisionlist0.begin(); iter != pCollisionlist0.end(); ++iter)
	{

		if (m_pCollisionBody->Push_Collision((CCollisionSphere*)(*iter)))
		{
			_vec3 vDir = m_pCollisionBody->Get_Direction();
			//m_dwIndex = m_pTransformCom->Go_Straight(fTimeDelta * vDir.x * -10.f, m_dwIndex);
			m_dwIndex = m_pTransformCom->Go_Right(fTimeDelta * vDir.z * -10.f, m_dwIndex);
			return;
		}
	}
}

void CMonster::Give_ManaToken(void)
{
	Engine::CGameObject* pPresent = CPresent::Create(m_pGraphicDev);
	if (nullptr == pPresent)
		return;

	dynamic_cast<CPresent*>(pPresent)->Set_Dir(_vec3(10.f, 5.f, 20.f));
	pPresent->Set_Pos(&(m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));

	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pPresent);
}

void CMonster::Free(void)
{

	Engine::Safe_Release(m_pInfoSubject);
	Engine::Safe_Release(m_pCollisionManager);
	Engine::Safe_Release(m_pNavMgr);
	CDynamicObject::Free();
}
