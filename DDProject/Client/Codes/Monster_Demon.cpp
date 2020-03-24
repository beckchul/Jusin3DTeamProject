#include "stdafx.h"
#include "Monster_Demon.h"
#include "Renderer.h"
#include "InfoSubject.h"
#include "PlayerObserver.h"
#include "CollisionManager.h"
#include "NavMgr.h"
#include "SplashMissile.h"
#include "Object_Manager.h"
#include "SplashDamage.h"
#include "Text_ToLobby.h"

#include "EffectMgr.h"

#include "DataManager.h"
#include "SoundMgr.h"
USING(Engine)

CMonster_Demon::CMonster_Demon(LPDIRECT3DDEVICE9 pGraphicDev)
: CMonster(pGraphicDev)
{
}

CMonster_Demon::~CMonster_Demon()
{
}

HRESULT CMonster_Demon::Ready_GameObject(_vec3 & vPosition)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(3.f, 3.f, 3.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPosition);
	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(90.f), 0.f));
	m_tInfo = INFO(5000, 5000, 0, 0, 70, false, 60.f, 0.f, 0.f);

	m_pPlayerObserver = CPlayerObserver::Create();
	m_pInfoSubject->Subscribe_Observer(m_pPlayerObserver);
	Load_AnimationInfo("../Resources/Data/AnimationData/DemonData.xml");

	// NavMesh
	m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_eMonsterType = MONSTER_DEMON;

	m_eMainState = MONSTER_CINEMATIC_LANDING;

	m_pInfoSubject->Add_Data(MESSAGE_BOSS_APPEAR, &m_tInfo.bDead); // 보스가 등장했음을 notify
	m_pInfoSubject->Add_Data(MESSAGE_BOSS_HP, &m_tInfo.iHp);
	m_pInfoSubject->Add_Data(MESSAGE_STAGE_CLEAR, &m_tInfo.bDead);
	m_vecTransColor = _vec4(0.7f, -0.3f, -0.3f, 0.f);

	m_listLHand = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Demon_Hand"
		, m_pDynamicMeshCom->Get_FrameMatrix("b_HandL")
		, m_pTransformCom->Get_WorldMatrix());

	m_listRHand = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Demon_Hand"
		, m_pDynamicMeshCom->Get_FrameMatrix("b_HandR")
		, m_pTransformCom->Get_WorldMatrix());

	m_fDeadTime = -1.f;
	CSoundMgr::GetInstance()->StopSound(CHANNEL_STAGE);
	CSoundMgr::GetInstance()->PlayBGM(L"BGM2.mp3", Engine::CHANNEL_STAGE, 0.3f);


	if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_DEMON))
		CSoundMgr::GetInstance()->Play_Sound(L"cromcruach_threat_02.wav", Engine::CHANNEL_DEMON, 0.8f);

	return NOERROR;
}

int CMonster_Demon::FirstUpdate_GameObject(const float & fTimeDelta)
{
	return 0;
}

int CMonster_Demon::Update_GameObject(const float & fTimeDelta)
{
	if (m_tInfo.bDead)
	{
		if (-1.f == m_fDeadTime)
		{
			m_fDeadTime = 0.f;
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", CText_ToLobby::Create(m_pGraphicDev));

			// 모든 몬스터 죽이기, 보스 죽는 사운드, 승리의 사운드
			m_pInfoSubject->Notify_Message(MESSAGE_STAGE_CLEAR);
			
			_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
			float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
			if (fDistance < 400.f)
			{
				if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_KOBOLD))
					CSoundMgr::GetInstance()->Play_Sound(L"Ogre_DeathFall.ogg", Engine::CHANNEL_KOBOLD, 0.8f);

				CSoundMgr::GetInstance()->StopSound(CHANNEL_STAGE);
				CSoundMgr::GetInstance()->Play_Sound(L"DD_Victory.ogg", Engine::CHANNEL_DEMON, 0.3f);
			}
		}

		m_fDeadTime += fTimeDelta;

		//m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(3.f - m_fDeadTime * 1.5f, 3.f - m_fDeadTime * 1.5f, 3.f - m_fDeadTime * 1.5f));

		if (m_fDeadTime >= 10.f)
		{
			CInfoSubject::GetInstance()->Notify_Message(MESSAGE_BOSS_APPEAR); // 보스 죽었음을 notify
			Give_ManaToken();	
			return 1;
		}
	}

	m_pInfoSubject->Notify_Message(MESSAGE_BOSS_HP);

	m_fTimeDelta = fTimeDelta;
	Change_State();
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DYNAMICMESH_NONEALPHA, this);

	if (-1 == m_dwIndex || m_bPreFly != m_bFly)
	{
		m_bPreFly = m_bFly;
		m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
	}
		
	if(!m_bFly)
		CMonster::Monster_Fall(fTimeDelta);
	else
	{
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 500.f)
		{
			if(FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_KOBOLD))
				CSoundMgr::GetInstance()->Play_Sound(L"Demon_WingFlap2.ogg", Engine::CHANNEL_KOBOLD, 0.8f);
		}
	}
	Play_MonsterPatton(fTimeDelta);
	Set_Collision();
	CMonster::Check_Collision(fTimeDelta);

	// 충돌 체크 후 색깔 지정.
	if (m_bColorAdd) // 만약 히트당했다면..
	{
		if (m_fColorTime <= m_fColorLimitTime)
		{
			m_fColorTime += fTimeDelta;
			m_vecColor = m_vecTransColor;
		}
		else
		{
			m_bColorAdd = false;
			m_fColorTime = 0.f;
			m_vecColor = _vec4(0.f, 0.f, 0.f, 0.f);
		}
	}

	return 0;
}

void CMonster_Demon::FirstRender_GameObject(_bool bTrue)
{
	if (!bTrue)
		CMonster::Play_Animation(m_fTimeDelta);
	m_pCollisionBody->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());
	m_pCollsionWeapon->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());
	m_pCollsionWeapon->Set_Damage((int)(m_tInfo.iAttPower * m_fLowDamage));
}

void CMonster_Demon::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);

	m_bFirstFrame = true;
}

HRESULT CMonster_Demon::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_Demon", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.CollisionSphere
	AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_MonBody", 12.f, m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), 3.f);
	AddCollisionSphere(m_pCollsionWeapon, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_MonWeapon", 9.f, m_pDynamicMeshCom->Get_FrameMatrix("b_HandL"), 3.f);
	m_pCollisionManager->Add_Component(L"Com_Col_MonBody", m_pCollisionBody);
	m_pCollisionManager->Add_Component(L"Com_Col_MonWeapon", m_pCollsionWeapon);
	m_pCollsionWeapon->Set_CheckCollsion(false);
	return NOERROR;
}

HRESULT CMonster_Demon::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pDynamicMeshCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	pEffect->SetVector("g_fHitColor", &m_vecColor);

	return NOERROR;
}

void CMonster_Demon::Play_MonsterPatton(const float& fTimeDelta)
{
	if (MONSTER_CINEMATIC_END == m_eMainState ||
		MONSTER_CINEMATIC_LANDING == m_eMainState ||
		MONSTER_DEAD == m_eMainState ||
		TRUE == m_tInfo.bDead)
		return;

	m_fCount += fTimeDelta;
	if (m_fCount >= 1.f)
	{
		++m_iPattonCount;
		m_fCount = 0.f;
	}

	if (m_iPattonCount == 2 && m_fCount == 0.f)
	{
		m_eMainState = MONSTER_FLY;
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 500.f)
		{
			if(FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_KOBOLD))
				CSoundMgr::GetInstance()->Play_Sound(L"Demon_WingFlap2.ogg", Engine::CHANNEL_KOBOLD, 0.8f);
		}
	}
	else if (m_iPattonCount >= 4 && m_pTransformCom->Get_Info(CTransform::INFO_POSITION).y <= 70.f && m_iPattonCount < 7)
		m_pTransformCom->Go_Down(2.f);
	else if (m_iPattonCount >= 7 && m_iPattonCount <= 12)
	{
		m_eMainState = MONSTER_MOVE_LEFT;
		m_pTransformCom->Go_FlyRight(-1.f * m_tInfo.fSpeedX * fTimeDelta);
		m_pTransformCom->Go_FlyStraight(0.5f * m_tInfo.fSpeedX * fTimeDelta);
	}
	else if (m_iPattonCount >= 13 && m_iPattonCount <= 14)
	{
		Search_Player(fTimeDelta);
		m_eMainState = MONSTER_IDLE;
	}
	else if (m_iPattonCount >= 15 && m_iPattonCount <= 19 && m_fCount == 0)
	{
		Search_Player(fTimeDelta);
		m_eMainState = MONSTER_ATTACK;
	}
	else if (m_iPattonCount == 20)
	{
		Search_Player(fTimeDelta);
		m_eMainState = MONSTER_IDLE;
	}
	else if (m_iPattonCount >= 21 && m_iPattonCount <= 26)
	{
		m_tFllowTarget.eMonsterTurn = MONSTER_TURN_END;
		m_eMainState = MONSTER_ATTACK_4;
		m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(135.f), 0.f));

		m_pTransformCom->Go_FlyStraight(1.f * m_tInfo.fSpeedX * fTimeDelta);
	}
	else if (m_iPattonCount == 27)
	{
		m_eMainState = MONSTER_ATTACK_3;
		m_bFly = false;
	}
	else if (m_iPattonCount == 29 && m_fCount == 0)
		m_eMainState = MONSTER_IDLE;
	else if (m_iPattonCount == 31 && m_fCount == 0)
		m_eMainState = MONSTER_ATTACK_5;
	else if (m_iPattonCount == 33 && m_fCount == 0)
		m_eMainState = MONSTER_ATTACK_6;
	else if (m_iPattonCount == 35 && m_fCount == 0)
	{
		m_eMainState = MONSTER_FLY;
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 500.f)
		{
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_KOBOLD))
				CSoundMgr::GetInstance()->Play_Sound(L"Demon_WingFlap2.ogg", Engine::CHANNEL_KOBOLD, 0.8f);
		}
	}
	else if (m_iPattonCount >= 37 && m_pTransformCom->Get_Info(CTransform::INFO_POSITION).y <= 65.f && m_iPattonCount < 40)
		m_pTransformCom->Go_Down(2.f);
	else if (m_iPattonCount >= 40 && m_iPattonCount <= 42)
	{
		m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(180.f), 0.f));
		m_pTransformCom->Go_FlyStraight(2.f * m_tInfo.fSpeedX * fTimeDelta);
		m_eMainState = MONSTER_MOVE;
	}
	else if (m_iPattonCount >= 43 && m_iPattonCount <= 44)
	{
		Search_Player(fTimeDelta);
		m_eMainState = MONSTER_IDLE;
	}
	else if (m_iPattonCount >= 45 && m_iPattonCount <= 49 && m_fCount == 0)
	{
		Search_Player(fTimeDelta);
		m_eMainState = MONSTER_ATTACK;
	}
	else if (m_iPattonCount >= 52 && m_iPattonCount <= 55)
	{
		m_tFllowTarget.eMonsterTurn = MONSTER_TURN_END;
		m_eMainState = MONSTER_MOVE;
		m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(-45.f), 0.f));
		m_pTransformCom->Go_FlyStraight(m_tInfo.fSpeedX * fTimeDelta);
	}
	else if (m_iPattonCount >= 56 && m_iPattonCount <= 57)
	{
		m_eMainState = MONSTER_ATTACK_4;
		m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(-45.f), 0.f));
		m_pTransformCom->Go_FlyStraight(m_tInfo.fSpeedX * fTimeDelta);
	}
	else if (m_iPattonCount == 58)
	{
		m_eMainState = MONSTER_ATTACK_3;
		m_bFly = false;
	}
	else if (m_iPattonCount == 60 && m_fCount == 0)
		m_eMainState = MONSTER_IDLE;
	else if (m_iPattonCount == 61 && m_fCount == 0)
		m_eMainState = MONSTER_ATTACK_5;
	else if (m_iPattonCount == 63 && m_fCount == 0)
		m_eMainState = MONSTER_ATTACK_6;
	else if (m_iPattonCount == 65 && m_fCount == 0)
	{
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 500.f)
		{
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_KOBOLD))
				CSoundMgr::GetInstance()->Play_Sound(L"cromcruach_threat_01.wav", Engine::CHANNEL_KOBOLD, 0.8f);
		}
		m_eMainState = MONSTER_ATTACK;
	}
	else if (m_iPattonCount == 71 && m_fCount == 0)
	{
		m_eMainState = MONSTER_FLY;
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 500.f)
		{
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_KOBOLD))
				CSoundMgr::GetInstance()->Play_Sound(L"Demon_WingFlap2.ogg", Engine::CHANNEL_KOBOLD, 0.8f);
		}
	}
	else if (m_iPattonCount >= 73 && m_pTransformCom->Get_Info(CTransform::INFO_POSITION).y <= 50.f && m_iPattonCount < 76)
		m_pTransformCom->Go_Down(2.f);
	else if (m_iPattonCount >= 76 && m_iPattonCount <= 77)
	{
		Search_Player(fTimeDelta);
		m_eMainState = MONSTER_IDLE;
	}
	else if (m_iPattonCount >= 78 && m_iPattonCount <= 83 && m_fCount == 0)
	{
		m_eMainState = MONSTER_ATTACK;
		Search_Player(fTimeDelta);
	}
	else if (m_iPattonCount == 85 && m_fCount == 0)
	{
		m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(90.f), 0.f));
		m_tFllowTarget.eMonsterTurn = MONSTER_TURN_END;
		m_eMainState = MONSTER_ATTACK_3;
		m_bFly = false;
	}
	else if (m_iPattonCount >= 88 && m_iPattonCount < 90)
	{
		m_eMainState = MONSTER_MOVE;
		m_dwIndex = m_pTransformCom->Go_Straight(0.5f * m_tInfo.fSpeedX * fTimeDelta, m_dwIndex);
	}
	else if (m_iPattonCount == 90 && m_fCount == 0)
	m_eMainState = MONSTER_ATTACK_6;
	else if (m_iPattonCount == 92 && m_fCount == 0)
		m_eMainState = MONSTER_ATTACK_5;
	else if (m_iPattonCount == 94 && m_fCount == 0)
	{
		m_eMainState = MONSTER_FLY;
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 500.f)
		{
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_KOBOLD))
				CSoundMgr::GetInstance()->Play_Sound(L"Demon_WingFlap2.ogg", Engine::CHANNEL_KOBOLD, 0.8f);
		}
	}
	else if (m_iPattonCount >= 96 && m_pTransformCom->Get_Info(CTransform::INFO_POSITION).y <= 50.f && m_iPattonCount < 99)
		m_pTransformCom->Go_Down(2.f);
	else if (m_iPattonCount >= 100 && m_iPattonCount <= 103)
	{
		m_pTransformCom->Go_FlyStraight(-1.f * m_tInfo.fSpeedX * fTimeDelta);
		m_eMainState = MONSTER_IDLE;
	}
	else if (m_iPattonCount >= 104 && m_iPattonCount <= 109 && m_fCount == 0)
	{
		m_eMainState = MONSTER_ATTACK;
		Search_Player(fTimeDelta);
	}
	else if (m_iPattonCount >= 110 && m_iPattonCount <= 114)
	{
		m_eMainState = MONSTER_MOVE_RIGHT;
		m_pTransformCom->Go_FlyRight(1.f * m_tInfo.fSpeedX * fTimeDelta);
		m_pTransformCom->Go_FlyStraight(0.5f * m_tInfo.fSpeedX * fTimeDelta);
	}
	else if (m_iPattonCount >= 115 && m_iPattonCount <= 117 && m_pTransformCom->Get_Info(CTransform::INFO_POSITION).y >= -100.f)
	{
		m_pTransformCom->Go_Down(-1.f);
	}
	else if (m_iPattonCount >= 118 && m_iPattonCount <= 119)
	{
		m_bFly = false;
		m_eMainState = MONSTER_IDLE;
		m_tInfo.iHp = 1;
	}
	else if (m_iPattonCount == 120 && m_fCount == 0)
		m_eMainState = MONSTER_ATTACK;
	else if(m_iPattonCount >= 124)
		m_eMainState = MONSTER_IDLE;
}

void CMonster_Demon::Search_Player(const float & fTimeDelta)
{
	_float fLegth;
	_float fMinLegth = 600.f;
	_vec3 vecMinTargetDir;
	_matrix matMinSourMatrix;

	for (int i = PLAYER_1; i < PLAYER_INDEX_END; ++i)				// 플레이어
	{
		const PLAYER_INFO* pPlayerInfo = m_pPlayerObserver->Get_PlayerInfo(i);

		if (nullptr == pPlayerInfo || pPlayerInfo->tUnitInfo.bDead)
			continue;

		_matrix* pDestMatrix = pPlayerInfo->pTransform->Get_WorldMatrix();
		_matrix pSourMatrix = m_pCollisionBody->Get_WorldMatrix();

		_vec3 vecDestPosition, vecSourPosition;
		memcpy_s(&vecDestPosition, sizeof(_vec3), &(pDestMatrix->m[3][0]), sizeof(_vec3));
		memcpy_s(&vecSourPosition, sizeof(_vec3), &(pSourMatrix.m[3][0]), sizeof(_vec3));

		_vec3 vecTargetDir = (vecDestPosition - vecSourPosition);

		fLegth = D3DXVec3Length(&vecTargetDir);
		if (fMinLegth > fLegth)
		{
			fMinLegth = fLegth;
			vecMinTargetDir = vecTargetDir;
			matMinSourMatrix = pSourMatrix;
			m_tFllowTarget.vTargetDir = vecMinTargetDir;
			D3DXVec3Normalize(&m_tFllowTarget.vTargetDir, &m_tFllowTarget.vTargetDir);
		}
	}

	if (MONSTER_TURN_FINISH != m_tFllowTarget.eMonsterTurn)
	{
		// 기본 방향 회전
		_vec3 vInfoDir;
		memcpy_s(&vInfoDir, sizeof(_vec3), &(matMinSourMatrix.m[2][0]), sizeof(_vec3));
		D3DXVec3Normalize(&vInfoDir, &vInfoDir);

		_vec3 vLeftDir;
		D3DXVec3Cross(&vLeftDir, &vInfoDir, &_vec3(0.0f, 1.0f, 0.0f));
		D3DXVec3Normalize(&vecMinTargetDir, &vecMinTargetDir);

		if (D3DXVec3Dot(&vLeftDir, &vecMinTargetDir) < 0)
		{
			//Left
			m_pTransformCom->Rotation(CTransform::ANGLE_Y, fTimeDelta * D3DXToRadian(180.f));
			if (MONSTER_TURN_END == m_tFllowTarget.eMonsterTurn)
				m_tFllowTarget.eMonsterTurn = MONSTER_TURN_LEFT;
			else if (MONSTER_TURN_RIGHT == m_tFllowTarget.eMonsterTurn)
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
		//m_tFllowTarget.vTargetDir = vecMinTargetDir;
		D3DXVec3Normalize(&m_tFllowTarget.vTargetDir, &m_tFllowTarget.vTargetDir);
	}
}

void CMonster_Demon::Change_State(void)
{
	switch (m_eMainState)
	{
	case MONSTER_ATTACK:
		if(m_bFly)
			m_iRenderState = DEMON_FLY_FIREBALL;
		else
			m_iRenderState = DEMON_ATTACK_SUMMONFLAME;
		break;
	case MONSTER_ATTACK_4:
		if (m_bFly)
			m_iRenderState = DEMON_FLY_POUND_LOOP;
		break;
	case MONSTER_ATTACK_3:
			m_iRenderState = DEMON_FLY_POUND_END;
		break;
	case MONSTER_ATTACK_5:
		m_iRenderState = DEMON_ATTACK_LEFT;
		break;
	case MONSTER_ATTACK_6:
		m_iRenderState = DEMON_ATTACK_RIGHT;
		break;
	case MONSTER_DEAD:
		m_iRenderState = DEMON_DEATH;
		break;
	case MONSTER_TURN:
		m_iRenderState = DEMON_TURN_LEFT;
		break;
	case MONSTER_FLY:
		m_iRenderState = DEMON_FLY_LAUNCH;
		break;
	case MONSTER_MOVE_LEFT:
		if (m_bFly)
			m_iRenderState = DEMON_FLY_LEFT;
		break;
	case MONSTER_MOVE_RIGHT:
		if (m_bFly)
			m_iRenderState = DEMON_FLY_RIGHT;
		break;
	case MONSTER_BACK_MOVE:
		if (m_bFly)
			m_iRenderState = DEMON_FLY_BACKWARD;
		break;
	case MONSTER_MOVE:
		if (m_bFly)
			m_iRenderState = DEMON_FLY_FORWARD;
		else
			m_iRenderState = DEMON_MOVE_FORWARD;
		break;
	case MONSTER_CINEMATIC_LANDING:
		m_iRenderState = DEMON_CINEMATIC_LANDING;
		break;
	case MONSTER_CINEMATIC_END:
		m_iRenderState = DEMON_CINEMATIC_CUTTHROAT;
		break;
	default:
		if (m_bFly)
			m_iRenderState = DEMON_FLY_HOVER;
		else
			m_iRenderState = DEMON_IDLE;
		break;
	}
}

void CMonster_Demon::Set_Collision(void)
{
	if (m_iRenderState == DEMON_FLY_FIREBALL && m_pDynamicMeshCom->GetTrackPosition() >= 0.7f && m_pDynamicMeshCom->GetTrackPosition() <= 0.9f)
	{
		if (m_iAttackState == 0)
		{
			++m_iAttackState;

			_matrix* matHand = m_pDynamicMeshCom->Get_FrameMatrix("b_HandR");
			(*matHand) *= (*m_pTransformCom->Get_WorldMatrix());

			D3DXVec3Normalize(&m_tFllowTarget.vTargetDir, &m_tFllowTarget.vTargetDir);
			_vec3 vHandPos(0.f, 0.f, 0.f);
			memcpy(&vHandPos, &matHand->m[3][0], sizeof(_vec3));
			Engine::CGameObject*		pGameObject = nullptr;
			pGameObject = CSplashMissile::Create(m_pGraphicDev, vHandPos, m_tFllowTarget.vTargetDir, DEMON_SPLASH_MISSLIE);

			if (nullptr == pGameObject)
				return;
			((CSplashMissile*)pGameObject)->Set_Damage(m_tInfo.iAttPower);
			if (FAILED(Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
				return;

			_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
			float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
			if (fDistance < 300.f)
			{
				CSoundMgr::GetInstance()->Play_Sound(L"Demon_fireball.ogg", Engine::CHANNEL_KOBOLD, 0.8f);

				CSoundMgr::GetInstance()->Play_Sound(L"cromcruach_shout_short_03.wav", Engine::CHANNEL_DEMON, 0.8f);
			}
		}
	}
	if (m_iRenderState == DEMON_ATTACK_SUMMONFLAME && m_pDynamicMeshCom->GetTrackPosition() >= 1.7f && m_pDynamicMeshCom->GetTrackPosition() <= 1.9f)
	{
		if (m_iAttackState == 0)
		{
			++m_iAttackState;

			// Damage
			Engine::CGameObject*		pGameObject = nullptr;
			pGameObject = CSplashDamage::Create(m_pGraphicDev, m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION), DEMON_SPLASH_ATTACK);
			((CSplashDamage*)pGameObject)->Set_Damage(m_tInfo.iAttPower);
			Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject);
		}
	}

	if (m_iRenderState == DEMON_FLY_POUND_END && m_pDynamicMeshCom->GetTrackPosition() >= 0.6f && m_pDynamicMeshCom->GetTrackPosition() <= 0.9f)
		m_pCollsionWeapon->Set_CheckCollsion(true);
	else if (m_iRenderState == DEMON_ATTACK_LEFT && m_pDynamicMeshCom->GetTrackPosition() >= 0.6f && m_pDynamicMeshCom->GetTrackPosition() <= 0.9f)
	{
		m_pCollsionWeapon->Set_CheckCollsion(true);
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 300.f)
		{
			if(FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_KOBOLD))
				CSoundMgr::GetInstance()->Play_Sound(L"Demon_swordslice.ogg", Engine::CHANNEL_KOBOLD, 0.8f);

			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_DEMON))
				CSoundMgr::GetInstance()->Play_Sound(L"cromcruach_shout_short_01.wav", Engine::CHANNEL_DEMON, 0.8f);

		}
	}
	else if (m_iRenderState == DEMON_ATTACK_RIGHT && m_pDynamicMeshCom->GetTrackPosition() >= 0.6f && m_pDynamicMeshCom->GetTrackPosition() <= 0.9f)
	{
		m_pCollsionWeapon->Set_CheckCollsion(true);
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 300.f)
		{
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_KOBOLD))
				CSoundMgr::GetInstance()->Play_Sound(L"Demon_swordslice2.ogg", Engine::CHANNEL_KOBOLD, 0.8f);

			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_DEMON))
				CSoundMgr::GetInstance()->Play_Sound(L"cromcruach_shout_short_02.wav", Engine::CHANNEL_DEMON, 0.8f);
		}
	}
	else
		m_pCollsionWeapon->Set_CheckCollsion(false);
}

CMonster_Demon* CMonster_Demon::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 & vPosition)
{
	CMonster_Demon* pInstance = new CMonster_Demon(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPosition)))
	{
		MessageBox(0, L"CMonster_Demon Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CMonster_Demon::Free(void)
{
	Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_listLHand);
	Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_listRHand);

	m_pInfoSubject->UnSubscribe_Observer(m_pPlayerObserver);
	Safe_Release(m_pPlayerObserver);

	m_pInfoSubject->Remove_Data(MESSAGE_BOSS_APPEAR, &m_bDead);
	m_pInfoSubject->Remove_Data(MESSAGE_BOSS_HP, &m_tInfo.iHp);
	m_pInfoSubject->Remove_Data(MESSAGE_STAGE_CLEAR, &m_tInfo.bDead);
	m_pCollisionManager->Delete_Component(L"Com_Col_MonBody", m_pCollisionBody);
	m_pCollisionManager->Delete_Component(L"Com_Col_MonWeapon", m_pCollsionWeapon);
	Engine::Safe_Release(m_pCollisionBody);
	Engine::Safe_Release(m_pCollsionWeapon);
	CMonster::Free();
}
