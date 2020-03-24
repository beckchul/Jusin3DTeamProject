#include "stdafx.h"
#include "Player_Monk.h"
#include "Renderer.h"
#include "InfoSubject.h"
#include "Player_Spear.h"
#include "NavMgr.h"
#include "CollisionManager.h"
#include "InputDev.h"
#include "SoundMgr.h"
#include "Camera_Manager.h"

// Missile
#include "Missile.h"
// Tower
#include "FireTower.h"

#include "Object_Manager.h"

// Effect
#include "EffectMgr.h"
#include "Effect_Trail.h"

#include "Font_Damage.h"

USING(Engine)

CPlayer_Monk::CPlayer_Monk(LPDIRECT3DDEVICE9 pGraphicDev)
: CPlayer(pGraphicDev)
{

}

CPlayer_Monk::~CPlayer_Monk()
{

}

HRESULT CPlayer_Monk::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &(_vec3(0.4f, 0.4f, 0.4f)));
	switch (g_eCurrentScene)
	{
	case SCENE_STAGE:
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-6.f, -12.f, -120.f)));
		break;
	case SCENE_STAGE2:
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-503.f, 120.f, 256.f)));
		break;
	}

	m_pWeaponMatrix = m_pDynamicMeshCom->Get_FrameMatrix("b_HandR");

	Load_AnimationInfo("../Resources/Data/AnimationData/MonkData.xml");

	m_pHighFilterBone[0] = "b_Spine2";
	m_pHighFilterBone[1] = ".";
	m_pHighFilterBone[2] = ".";

	m_pLowFilterBone[0] = "b_HipR";
	m_pLowFilterBone[1] = "b_HipL";
	m_pLowFilterBone[2] = ".";

	// NavMesh
	m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	//// 무기
	m_pWeapon = CPlayer_Spear::Create(m_pGraphicDev);
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon->Set_MatrixInfo(m_pTransformCom->Get_WorldMatrix(), m_pWeaponMatrix);
	m_pWeapon->Create_Sphere();

	m_vecTransColor = _vec4(1.f, -0.3f, -0.3f, 0.f);
	
	return NOERROR;
}

// 플레이어의 FirstUpdate는 멤버 변수로 처리할 것. (뒤늦게 생성되면 Layer에서 First_Update를 못탐)
int CPlayer_Monk::FirstUpdate_GameObject(const float & fTimeDelta)
{
	if (TRUE == m_bFirstUpdate)
		return 0;

	m_bFirstUpdate = TRUE;

	m_pPlayerInfo = new PLAYER_INFO(m_iClinetNum, m_pTransformCom, m_pWeaponMatrix, PLAYER_MONK, PLAYER_END, 123, 1, 0.f, INFO(300, 300, 0, 300, 35, false, 0.f, 0.f, 0.f));
	CPlayer::Observer_AddData();

	return 0;
}

int CPlayer_Monk::Update_GameObject(const float& fTimeDelta)
{
	FirstUpdate_GameObject(fTimeDelta);

	if (m_bExitServer) // 접속 종료
		return 1;

	if (m_pPlayerInfo->tUnitInfo.bDead)
	{
		switch (g_eCurrentScene)
		{
		case SCENE_STAGE:
			m_fDeadTime += m_fTimeDelta;
			if (m_fDeadTime >= 3)
			{
				m_fDeadTime = 0.f;
				m_pPlayerInfo->tUnitInfo.bDead = false;
				m_pPlayerInfo->eMainState = PLAYER_RESPONE;
				m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-6.f, 20.f, -120.f)));
				m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
				m_pPlayerInfo->tUnitInfo.iHp = m_pPlayerInfo->tUnitInfo.iHpMax;
				m_pPlayerInfo->tUnitInfo.iMp = 0;

				Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Alive", &_vec3(-6.f, 15.f, -120.f));

				CCamera_Manager::GetInstance()->Change_Camera(CAMERA_STATIC, 0, -1.f);
			}
			else if (m_fDeadTime >= 1.f && m_bMyClient)
			{
				m_bBackColorAdd = true;
			}
			break;
		case SCENE_STAGE2:
			return 1;
		}
	}

	m_fTimeDelta = fTimeDelta;
	Change_State();
	Set_Collision();

	if (m_bMyClient) // 내 플레이어
	{
		_vec4 backColor = CDataManager::GetInstance()->Get_BackColor();

		// 점점 밝아짐
		if (!m_bBackColorAdd)
		{
			if (backColor.x > 0.f)
			{
				backColor -= _vec4(1.f, 1.f, 1.f, 1.f) * m_fTimeDelta * 1.5f;
				CDataManager::GetInstance()->Set_BackColor(backColor);
			}
		}

		// 점점 어두워짐
		if (m_bBackColorAdd)
		{
			if (backColor.x <= 1.f)
			{
				backColor += _vec4(1.f, 1.f, 1.f, 1.f) * m_fTimeDelta * 0.5f; // 2초간
				CDataManager::GetInstance()->Set_BackColor(backColor);
			}
			else
				m_bBackColorAdd = false;
		}

		if (!m_pPlayerInfo->tUnitInfo.bDead && PLAYER_DEAD != m_pPlayerInfo->eMainState
			&& PLAYER_RESPONE != m_pPlayerInfo->eMainState)
		KeyCheck(fTimeDelta);
		else if (PLAYER_DEAD == m_pPlayerInfo->eMainState)
			KeyCheck_Death();

		CPlayer::Player_Fall(fTimeDelta); // 중력

		CPlayer::Player_Check_SecondFloor();

		if(-1 == m_dwIndex)
			m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

		Engine::CGameObject::Update_GameObject(fTimeDelta); // Transform Update

		SendLocalData(); // 자신의 정보(WorldMatrix)를 서버로 보낸다
	}
	else // 다른 플레이어들
	{
		m_pTransformCom->Set_WorldMatrix(&m_tLocalData.matWorld);

		_vec3 vecPos;
		memcpy(&vecPos, m_tLocalData.matWorld.m[3], sizeof(_vec3));
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vecPos);

		m_iHighRenderState = m_tLocalData.iHighAnimation;
		m_iLowRenderState = m_tLocalData.iLowAnimation;
		m_pPlayerInfo->eMainState = (PLAYER_MAIN_STATE)m_tLocalData.eMainState;
		m_eBuildTower = (TOWER_TYPE)m_tLocalData.wSubMessage;
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DYNAMICMESH_NONEALPHA, this);

	CPlayer::Observer_Update();
	CPlayer::Check_Collision();
	CPlayer::Healing();

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
	
	// Tower
	CPlayer::Preview_Tower(fTimeDelta); // 타워 미리보기
	CPlayer::Preview_Tower2();
	CPlayer::Summoning_Tower(fTimeDelta); // 타워 소환중... (끝나면 Build_Tower)
	CPlayer::Build_Tower();
	CPlayer::Cancel_Tower(); // 타워 취소

	return 0;
}

void CPlayer_Monk::FirstRender_GameObject(_bool bTrue)
{
	if (bTrue)
		CPlayer::Play_Animation(0.f);
	else
		CPlayer::Play_Animation(m_fTimeDelta);
	
	m_pCollisionBody->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());

}

void CPlayer_Monk::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return; 
	
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	if (m_pPlayerInfo->tUnitInfo.bDead)
	{
		m_pDynamicMeshCom->Set_TrackPosition(m_iDeadTrackIndex, m_fDeadTrackPosition);
		m_pDynamicMeshCom->Play_AnimationSet(0.f);
		m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);
	}
	else
	{
		if (nullptr != m_pWeapon)
			m_pWeapon->Update_GameObject(m_fTimeDelta);
		m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);
	}
		

	m_bFirstFrame = true;
}

HRESULT CPlayer_Monk::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*         pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_Monk", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.CollisionSphere
	AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Body", 30.f, m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), 0.4f);
	m_pCollisionManager->Add_Component(L"Com_Col_Body", m_pCollisionBody);
	return NOERROR;
}

HRESULT CPlayer_Monk::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

void CPlayer_Monk::Change_State(void)
{
	if (PLAYER_HURT == m_pPlayerInfo->eMainState)
		m_iHighRenderState = MONK_HURT;
	else if (PLAYER_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK_SECOND == m_pPlayerInfo->eMainState)
		m_iHighRenderState = MONK_MELLE_ATTACK2;
	else if (PLAYER_ATTACK == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK == m_pPlayerInfo->eMainState)
		m_iHighRenderState = MONK_MELLE_ATTACK1;
	else if (PLAYER_TURN == m_pPlayerInfo->eMainState)
		m_iHighRenderState = MONK_TRUN_LEFT;
	else if (PLAYER_ATTACK_THIRD == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK_THIRD == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK_THIRD == m_pPlayerInfo->eMainState)
		m_iHighRenderState = MONK_FIRE;
	else if (PLAYER_JUMP == m_pPlayerInfo->eMainState)
		m_iHighRenderState = MONK_JUMP;
	else if (PLAYER_JUMP_FALLING == m_pPlayerInfo->eMainState)
		m_iHighRenderState = MONK_JUMP_FALLING;
	else if (PLAYER_HEAL == m_pPlayerInfo->eMainState)
		m_iHighRenderState = MONK_HEAL;
	else if (PLAYER_DEAD == m_pPlayerInfo->eMainState)
		m_iHighRenderState = MONK_DEATH;
	else if (PLAYER_RESPONE == m_pPlayerInfo->eMainState)
		m_iHighRenderState = MONK_SPAWN;
	else
		m_iHighRenderState = MONK_IDLE;

	if (PLAYER_HURT == m_pPlayerInfo->eMainState)
		m_iLowRenderState = MONK_HURT;
	else if (PLAYER_JUMP == m_pPlayerInfo->eMainState)
		m_iLowRenderState = MONK_JUMP;
	else if (PLAYER_JUMP_FALLING == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK == m_pPlayerInfo->eMainState
		|| PLAYER_JUMP_FALLING_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK_THIRD == m_pPlayerInfo->eMainState 
		|| PLAYER_JUMP_ATTACK == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK_THIRD == m_pPlayerInfo->eMainState)
		m_iLowRenderState = MONK_JUMP_FALLING;
	else if (PLAYER_HEAL == m_pPlayerInfo->eMainState)
		m_iLowRenderState = MONK_HEAL;
	else if (PLAYER_DEAD == m_pPlayerInfo->eMainState)
		m_iLowRenderState = MONK_DEATH;
	else if (PLAYER_RESPONE == m_pPlayerInfo->eMainState)
		m_iLowRenderState = MONK_SPAWN;
	else
		m_iLowRenderState = MONK_IDLE;

	// Tower
	if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState)
	{
		m_iHighRenderState = MONK_SUMMON;
		m_iLowRenderState = MONK_SUMMON;
	}
	else if (PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState)
	{
		m_iHighRenderState = MONK_SUMMON_PLACE;
		m_iLowRenderState = MONK_SUMMON_PLACE;
	}
	else if (PLAYER_TOWER_CANCEL == m_pPlayerInfo->eMainState)
	{
		m_iHighRenderState = MONK_SUMMON_STOP;
		m_iLowRenderState = MONK_SUMMON_STOP;
	}
}

void CPlayer_Monk::KeyCheck(const float& fTimeDelta)
{
	int      iDistance = 0;
	m_fTrunCount -= fTimeDelta * 10.f;

	if (iDistance = m_pInputDev->GetDIMouseMove(Engine::CInputDev::DIM_X))
	{
		if (10 < iDistance || -10 > iDistance)
		{
			m_fTrunCount = 1.f;
		}
		if (m_fTrunCount > 0.f)
		{
			if (ATTACK_HURT_CHECK && JUMP_CHECK)
			{
				if (PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState && PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState
					&& PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState && PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState
					&& PLAYER_HEAL != m_pPlayerInfo->eMainState) // jsh
				{
					m_pPlayerInfo->eMainState = PLAYER_TURN;
					m_iHighRenderState = MONK_TRUN_LEFT;
				}
				
			}
			if (JUMP_CHECK && PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState && PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState
				&& PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState && PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState
				&& PLAYER_HEAL != m_pPlayerInfo->eMainState)
				m_iLowRenderState = MONK_TRUN_LEFT;
		}
		m_pTransformCom->Rotation(CTransform::ANGLE_Y, D3DXToRadian(iDistance / 10.f));
	}

	Move_MouseAngle();

	// WASD Space
	if (PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState &&  // jsh 못 움직이는 조건
		PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState && 
		PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState &&
		PLAYER_HEAL != m_pPlayerInfo->eMainState) // 못 움직이는 조건 jsh
	{
		// A (왼쪽)
		if (m_pInputDev->StayKeyDown(DIK_A))
		{
			if (ATTACK_HURT_CHECK && JUMP_CHECK)
			{
				m_pPlayerInfo->eMainState = PLAYER_MOVE;
				m_iHighRenderState = MONK_MOVE_LEFT;
			}
			if (JUMP_CHECK)
				m_iLowRenderState = MONK_MOVE_LEFT;
			m_pPlayerInfo->tUnitInfo.fSpeedX = -50.f;
		}

		// D (오른쪽)
		if (m_pInputDev->StayKeyDown(DIK_D)) 
		{
			if (ATTACK_HURT_CHECK && JUMP_CHECK)
			{
				m_pPlayerInfo->eMainState = PLAYER_MOVE;
				m_iHighRenderState = MONK_MOVE_RIGHT;
			}
			if (JUMP_CHECK)
				m_iLowRenderState = MONK_MOVE_RIGHT;
			m_pPlayerInfo->tUnitInfo.fSpeedX = 50.f;
		}

		// W (앞)
		if (m_pInputDev->StayKeyDown(DIK_W))
		{
			if (ATTACK_HURT_CHECK && JUMP_CHECK)
			{
				m_iHighRenderState = MONK_RUN_FORWARD;
				m_pPlayerInfo->eMainState = PLAYER_MOVE;
			}
			if (JUMP_CHECK)
				m_iLowRenderState = MONK_RUN_FORWARD;
			m_pPlayerInfo->tUnitInfo.fSpeedZ = 70.f;
		}
		// S(뒤)
		else if (m_pInputDev->StayKeyDown(DIK_S)) 
		{
			if (ATTACK_HURT_CHECK && JUMP_CHECK)
			{
				m_pPlayerInfo->eMainState = PLAYER_MOVE;
				m_iHighRenderState = MONK_MOVE_BACKWARD;
			}
			if (JUMP_CHECK)
				m_iLowRenderState = MONK_MOVE_BACKWARD;
			m_pPlayerInfo->tUnitInfo.fSpeedZ = -70.f;
		}
		// Space(점프)
		if (m_pInputDev->OnceKeyDown(DIK_SPACE)) 
		{
			if (ATTACK_HURT_CHECK)
			{
				m_pPlayerInfo->eMainState = PLAYER_JUMP;
				m_iHighRenderState = MONK_JUMP;
			}
			if (m_pPlayerInfo->eMainState != PLAYER_JUMP &&
				m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING)
				m_iLowRenderState = MONK_JUMP;
			CPlayer::Player_Jump(fTimeDelta);
			if(m_bMyClient)
				CSoundMgr::GetInstance()->Play_Sound(L"Monk_jumpVO3_2.ogg", Engine::CHANNEL_MONK, 0.6f);
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_T))
	{
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-6.f, 20.f, -120.f)));
		m_pPlayerInfo->eMainState = PLAYER_NOT_INPUT;
		m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
	}
	if (m_pInputDev->OnceKeyDown(DIK_Y))
	{
		m_pPlayerInfo->tUnitInfo.iMp = m_pPlayerInfo->tUnitInfo.iMpMax;
	}

	// 마우스 좌클릭
	if (m_pInputDev->OnceMouseClick(CInputDev::DIM_LBUTTON)) 
	{
		if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState) // 타워 미리보기 중
		{
			if (m_bAbleToBuild)
			{
				m_pPlayerInfo->eMainState = PLAYER_TOWER_SUMMONING; // Summon 시작

				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", Engine::CHANNEL_MONK, 0.5f);
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MAGE, 0.5f);
				switch (m_eBuildTower)
				{
				case TOWER_TYPE_SLOWAURA:
					CPlayer::Change_TowerColor(_vec4(0.3f, 0.7f, 0.3f, 1.f));
					break;
				case TOWER_TYPE_LIGHTNINGAURA:
					CPlayer::Change_TowerColor(_vec4(0.2f, 0.2f, 0.5f, 1.f));
					break;
				case TOWER_TYPE_HEALINGAURA:
					CPlayer::Change_TowerColor(_vec4(0.5f, 0.12f, 0.8f, 1.f));
					break;
				case TOWER_TYPE_DRAINAURA:
					break;
				case TOWER_TYPE_ENRAGEAURA:
					break;
				}
			}
		}
		else
		{
			if (ATTACK_CHECK && PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState &&
				PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState &&
				PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState &&
				PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState &&
				PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState &&
				PLAYER_HEAL != m_pPlayerInfo->eMainState)
			{
				if (PLAYER_JUMP == m_pPlayerInfo->eMainState)
					m_pPlayerInfo->eMainState = PLAYER_JUMP_ATTACK;
				else if (PLAYER_JUMP_FALLING == m_pPlayerInfo->eMainState)
					m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK;
				else
					m_pPlayerInfo->eMainState = PLAYER_ATTACK;
				
				m_iHighRenderState = MONK_MELLE_ATTACK1;
				++m_iAttackCount;
				_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
				float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
				if (fDistance < 200.f)
				{
					CSoundMgr::GetInstance()->Play_Sound(L"Monk_meleeswing_2.ogg", Engine::CHANNEL_MONK);
				}
			}
		}
	}
	CPlayer::Control_PlayerMove(fTimeDelta);

	// 마우스 우클릭
	if (m_pInputDev->OnceMouseClick(CInputDev::DIM_RBUTTON)) 
	{
		if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState) // 타워 취소
		{
			m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
		}
		else
		{
			if (ATTACK_CHECK)
			{
				m_pPlayerInfo->eMainState = PLAYER_ATTACK_THIRD;
				m_iHighRenderState = MONK_FIRE;
				_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
				float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
				if (fDistance < 200.f)
				{
					CSoundMgr::GetInstance()->Play_Sound(L"Monk_ChiBolt_2.ogg", Engine::CHANNEL_MONK);
				}
			}
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_1))
	{
		if (PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState &&
			PLAYER_HEAL != m_pPlayerInfo->eMainState &&
			m_pPlayerInfo->tUnitInfo.iMp > 0 
			&& m_pPlayerInfo->tUnitInfo.iHp < m_pPlayerInfo->tUnitInfo.iHpMax)
		{
			m_pPlayerInfo->eMainState = PLAYER_HEAL;
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_6))
	{
		if (m_pPlayerInfo->tUnitInfo.iMp >= 30)
		{
			if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
			else
			{
				// MONK_SUMMON
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW;
				m_eBuildTower = TOWER_TYPE_SLOWAURA;
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MAGE, 0.5f);
			}
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_7))
	{
		if (m_pPlayerInfo->tUnitInfo.iMp >= 50)
		{
			if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
			else
			{
				// MONK_SUMMON
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW;
				m_eBuildTower = TOWER_TYPE_LIGHTNINGAURA;
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MAGE, 0.5f);
			}
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_8))
	{
		if (m_pPlayerInfo->tUnitInfo.iMp >= 40)
		{
			if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
			else
			{
				// MONK_SUMMON
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW;
				m_eBuildTower = TOWER_TYPE_HEALINGAURA;
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MAGE, 0.5f);
			}
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_9))
	{
		if (m_pPlayerInfo->tUnitInfo.iMp >= 60)
		{
			if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
			else
			{
				// MONK_SUMMON
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW;
				m_eBuildTower = TOWER_TYPE_DRAINAURA;
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MAGE, 0.5f);
			}
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_0))
	{
		if (m_pPlayerInfo->tUnitInfo.iMp >= 100)
		{
			if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
			else
			{
				// MONK_SUMMON
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW;
				m_eBuildTower = TOWER_TYPE_ENRAGEAURA;
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MAGE, 0.5f);
			}
		}
	}

	// 레디 박아
	if (m_pInputDev->OnceKeyDown(DIK_E))
	{
		// 크리스탈 위치를 원점으로 가정했음
		if (D3DXVec3Length(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION)) < 30.f)
		{
			// 서버로 준비되었다고 보내줌
			Ready_Combat();

			CSoundMgr::GetInstance()->Play_Sound(L"Crystal_Activate.ogg", Engine::CHANNEL_CRYSTAL_ACTIVATE);
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_NUMPADSTAR))
	{
		cout << g_iClientIndex << endl;
	}

	if (m_pInputDev->OnceKeyDown(DIK_NUMPADSLASH))
	{
		_vec3 vecPlayer = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);

		cout << "플레이어" << vecPlayer.x << " " << vecPlayer.y << " " << vecPlayer.z << endl;
	}
}

void CPlayer_Monk::KeyCheck_Death(void)
{
	if (m_pInputDev->OnceKeyDown(DIK_SPACE))
	{
		CCamera_Manager::GetInstance()->Change_Camera(CAMERA_DYNAMIC);
	}
}

void CPlayer_Monk::Set_Collision()
{
	if (nullptr == m_pWeapon)
		return;

	if (m_iHighRenderState == MONK_MELLE_ATTACK1 && m_pDynamicMeshCom->GetTrackPosition() >= 0.2f && m_pDynamicMeshCom->GetTrackPosition() <= 0.7f)
	{
		m_pWeapon->Set_CollisionCheck(true);
		m_pWeapon->Set_Damage(m_pPlayerInfo->tUnitInfo.iAttPower);
	}
	else if (m_iHighRenderState == MONK_MELLE_ATTACK2 && m_pDynamicMeshCom->GetTrackPosition() >= 0.2f && m_pDynamicMeshCom->GetTrackPosition() <= 0.6f)
	{
		m_pWeapon->Set_CollisionCheck(true);
		m_pWeapon->Set_Damage(m_pPlayerInfo->tUnitInfo.iAttPower);
	}
	else
		m_pWeapon->Set_CollisionCheck(false);

	if (m_iHighRenderState == MONK_FIRE && m_pDynamicMeshCom->GetTrackPosition() >= 0.16f && m_pDynamicMeshCom->GetTrackPosition() <= 0.18f)
	{
		if (0 == m_iAttackCount)
		{
			m_iAttackCount += 2;

			_matrix* matLHand;
			_matrix* matParent;
			matLHand = m_pDynamicMeshCom->Get_FrameMatrix("b_HandL");
			matParent = m_pTransformCom->Get_WorldMatrix();
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Flare", matLHand, matParent);
			// 자신&부모 matrix받아와 fix하는식으로 수정

			*matLHand *= *matParent;

			_vec3 vHandPos(0.f, 0.f, 0.f);

			memcpy(&vHandPos, &matLHand->m[3][0], sizeof(_vec3));
			_vec3 vDir;
			if (m_bMyClient)
			{
				if (FALSE == m_pCollisionManager->Pick_MouseCol(vDir))
					return;

				m_vecPick = vDir;
			}
			else
				vDir = m_tLocalData.vecPick;
			
			D3DXVec3Normalize(&vDir, &(vDir - vHandPos));
			Engine::CGameObject*		pGameObject = nullptr;
			pGameObject = CMissile::Create(m_pGraphicDev, vHandPos, vDir, MONK_MISSILE);

			if (nullptr == pGameObject)
				return;
			((CMissile*)pGameObject)->Set_Damage(m_pPlayerInfo->tUnitInfo.iAttPower / 2);
			if (FAILED(Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
				return;
		}
	}
}

CPlayer_Monk* CPlayer_Monk::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer_Monk* pInstance = new CPlayer_Monk(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPlayer_Monk Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPlayer_Monk::Free(void)
{
	CPlayer::Observer_RemoveData();

	Engine::Safe_Delete(m_pPlayerInfo);
	m_pCollisionManager->Delete_Component(L"Com_Col_Body", m_pCollisionBody);
	CPlayer::Free();
}