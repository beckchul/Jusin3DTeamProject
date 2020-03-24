#include "stdafx.h"
#include "Player_Mage.h"
#include "Renderer.h"
#include "InfoSubject.h"
#include "Player_Staff.h"
#include "CollisionManager.h"
#include "NavMgr.h"
#include "InputDev.h"
#include "InfoSubject.h"
#include "SoundMgr.h"
#include "Camera_Manager.h"
#include "MousePointer.h"
#include "ClientNetwork.h"

#include "Effect.h"
#include "Effect_RectParticle.h"
#include "EffectMgr.h"

#include "SplashDamage.h"
#include "Object_Manager.h"
#include "DataManager.h"
#include "EasterEgg.h"

#include "Effect_Trail.h"
USING(Engine)

CPlayer_Mage::CPlayer_Mage(LPDIRECT3DDEVICE9 pGraphicDev)
: CPlayer(pGraphicDev)
{
	
}

CPlayer_Mage::~CPlayer_Mage()
{

}
HRESULT CPlayer_Mage::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &(_vec3(0.8f, 0.8f, 0.8f)));

	switch (g_eCurrentScene)
	{
	case SCENE_STAGE:
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-6.f, -12.f, 120.f)));
		break;
	case SCENE_STAGE2:
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-503.f, 120.f, 256.f)));
		break;
	case SCENE_LOBBY:
		break;
	}

	//m_pWeaponMatrix = m_pDynamicMeshCom->Get_FrameMatrix("b_Weapon");	//사기치고있네 ㅡㅡ
	m_pWeaponMatrix = m_pDynamicMeshCom->Get_FrameMatrix("b_FingersR");
	
	Load_AnimationInfo("../Resources/Data/AnimationData/MageData.xml");

	m_pHighFilterBone[0] = "b_ShoulderL";
	m_pHighFilterBone[1] = "b_ShoulderR";
	m_pHighFilterBone[2] = ".";
	m_pLowFilterBone[0] = "b_KneeL";
	m_pLowFilterBone[1] = "b_KneeR";
	m_pLowFilterBone[2] = "b_Cape";

	// 무기
	m_pWeapon = CPlayer_Staff::Create(m_pGraphicDev);
	if (nullptr == m_pWeapon)
		return E_FAIL;
	
	m_pWeapon->Set_MatrixInfo(m_pTransformCom->Get_WorldMatrix(), m_pWeaponMatrix);

	m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_vecTransColor = _vec4(1.f, -0.3f, -0.3f, 0.f);

	//Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Like_Spotlight", m_pTransformCom->Get_WorldMatrix());

	return NOERROR;
}

// 플레이어의 FirstUpdate는 멤버 변수로 처리할 것. (뒤늦게 생성되면 Layer에서 First_Update 기차를 못탐)
int CPlayer_Mage::FirstUpdate_GameObject(const float & fTimeDelta)
{
	if (TRUE == m_bFirstUpdate)
		return 0;

	m_bFirstUpdate = TRUE;

	m_pPlayerInfo = new PLAYER_INFO(m_iClinetNum, m_pTransformCom, m_pWeaponMatrix, PLAYER_MAGE, PLAYER_END, 123, 1, 0.f, INFO(200, 200, 0, 400, 30, false, 0.f, 0.f, 0.f));
	CPlayer::Observer_AddData();

	return 0;
}

int CPlayer_Mage::Update_GameObject(const float& fTimeDelta)
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
				m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-6.f, 30.f, 120.f)));
				m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
				m_pPlayerInfo->tUnitInfo.iHp = m_pPlayerInfo->tUnitInfo.iHpMax;
				m_pPlayerInfo->tUnitInfo.iMp = 0;

				Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Alive", &_vec3(-6.f, 25.f, 120.f));

				CCamera_Manager::GetInstance()->Change_Camera(CAMERA_STATIC, 0, -1.f);
				
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"Apprenctice_cheer1.ogg", CHANNEL_MAGE, 0.9f);
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
	m_fTrunCount -= fTimeDelta * 10.f;
	Change_State();

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

		if (-1 == m_dwIndex)
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
	Set_Collision();

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

void CPlayer_Mage::FirstRender_GameObject(_bool bTrue)
{
	if (bTrue)
		CPlayer::Play_Animation(0.f);
	else
		CPlayer::Play_Animation(m_fTimeDelta);

	m_pCollisionBody->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_Mage::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
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

HRESULT CPlayer_Mage::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_Mage", Engine::CComponent::COM_STATIC, L"Com_Mesh");
	
	// For.CollisionSphere
	AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Body", 16.f, m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), 0.8f);
	m_pCollisionManager->Add_Component(L"Com_Col_Body", m_pCollisionBody);

	return NOERROR;
}

HRESULT CPlayer_Mage::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

void CPlayer_Mage::Change_State(void)
{
	switch (m_pPlayerInfo->eMainState)		//상체 State
	{
	case PLAYER_HURT:
		m_iHighRenderState = MAGE_HURT;
		break;
	case PLAYER_ATTACK:
		m_iHighRenderState = MAGE_FIRE;
		break;
	case PLAYER_TURN: 
		m_iHighRenderState = MAGE_TRUN_LEFT;
		break;
	case PLAYER_JUMP:
		m_iHighRenderState = MAGE_JUMP;
		break;
	case PLAYER_JUMP_ATTACK:
	case PLAYER_JUMP_FALLING:
	case PLAYER_JUMP_FALLING_ATTACK:
		m_iHighRenderState = MAGE_JUMP_FALLING;
		break;
	case PLAYER_ATTACK_SECOND:
	case PLAYER_JUMP_ATTACK_SECOND:
	case PLAYER_JUMP_FALLING_ATTACK_SECOND:
	{
		m_iHighRenderState = MAGE_CHARGE_MIN_KNOCKBACK;
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 250.f)
		{
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MAGE_SKILL2) &&
				FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MAGE_SKILL))
				CSoundMgr::GetInstance()->Play_Sound(L"MagicStaff_Charging.ogg", Engine::CHANNEL_MAGE_SKILL, 0.7f);
		}
	}
		break;
	case PLAYER_ATTACK_THIRD:
	case PLAYER_JUMP_ATTACK_THIRD:
	case PLAYER_JUMP_FALLING_ATTACK_THIRD:
	{
		m_iHighRenderState = MAGE_CHARGE_MAX_KNOCKBACK;
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 250.f)
		{
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MAGE_SKILL2) &&
				FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MAGE_SKILL))
				CSoundMgr::GetInstance()->Play_Sound(L"MagicStaff_Charging.ogg", Engine::CHANNEL_MAGE_SKILL, 0.7f);
		}
	}
		break;
	case PLAYER_ATTACK_FOURTH:
	case PLAYER_JUMP_ATTACK_FOURTH:
	case PLAYER_JUMP_FALLING_ATTACK_FOURTH:
		m_iHighRenderState = MAGE_KNOCKBACK;
		break;
	case PLAYER_TOWER_PREVIEW:			//Tower
	case PLAYER_TOWER_PREVIEW_SECOND:
		m_iHighRenderState = MAGE_SUMMON;
		break;
	case PLAYER_TOWER_SUMMONING:
		m_iHighRenderState = MAGE_SUMMON_PLACE;
		break;
	case PLAYER_TOWER_CANCEL:
		m_iHighRenderState = MAGE_SUMMON_STOP;
		break;
	case PLAYER_SKILL:
		m_iHighRenderState = MAGE_DETONATE;
		break;
	case PLAYER_SKILL_ATTACK:
		m_iHighRenderState = MAGE_MANA_BOMB;
		break;
	case PLAYER_HEAL:
		m_iHighRenderState = MAGE_HEAL;
		break;
	case PLAYER_DEAD:
	{
		m_iHighRenderState = MAGE_DEATH;
		if (m_bIsCharging && m_Templist.size() > 0)
		{
			Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_Templist);
			m_bIsCharging = FALSE;
		}
		break;
	}
	case PLAYER_RESPONE:
		m_iHighRenderState = MAGE_SPAWN;
		break;
	default:
		m_iHighRenderState = MAGE_IDLE;
		break;
	}

	switch (m_pPlayerInfo->eMainState)	//하체 State
	{
	case PLAYER_HURT:
		m_iLowRenderState = MAGE_HURT;
		break;
	case PLAYER_ATTACK:
		m_iLowRenderState = MAGE_FIRE;
		break;
	case PLAYER_JUMP:
		m_iLowRenderState = MAGE_JUMP;
		break;
	case PLAYER_TURN:
		m_iLowRenderState = MAGE_TRUN_LEFT;
		break;
	case PLAYER_ATTACK_FOURTH:
	case PLAYER_JUMP_ATTACK_FOURTH:
	case PLAYER_JUMP_FALLING_ATTACK_FOURTH:
		m_iLowRenderState = MAGE_KNOCKBACK;
		break;
	case PLAYER_JUMP_ATTACK:
	case PLAYER_JUMP_FALLING:
	case PLAYER_JUMP_FALLING_ATTACK:
	case PLAYER_JUMP_ATTACK_SECOND:
	case PLAYER_JUMP_FALLING_ATTACK_SECOND:
	case PLAYER_JUMP_ATTACK_THIRD:
	case PLAYER_JUMP_FALLING_ATTACK_THIRD:
		m_iLowRenderState = MAGE_JUMP_FALLING;
		break;
	case PLAYER_TOWER_PREVIEW:			//Tower
	case PLAYER_TOWER_PREVIEW_SECOND:
		m_iLowRenderState = MAGE_SUMMON;
		break;
	case PLAYER_TOWER_SUMMONING:
		m_iLowRenderState = MAGE_SUMMON_PLACE;
		break;
	case PLAYER_TOWER_CANCEL:
		m_iLowRenderState = MAGE_SUMMON_STOP;
		break;
	case PLAYER_SKILL:
		m_iLowRenderState = MAGE_DETONATE;
		break;
	case PLAYER_SKILL_ATTACK:
		m_iLowRenderState = MAGE_MANA_BOMB;
		break;
	case PLAYER_HEAL:
		m_iLowRenderState = MAGE_HEAL;
		break;
	case PLAYER_DEAD:
		m_iLowRenderState = MAGE_DEATH;
		break;
	case PLAYER_RESPONE:
		m_iLowRenderState = MAGE_SPAWN;
		break;
	default:
		m_iLowRenderState = MAGE_IDLE;
		break;
	}
}

void CPlayer_Mage::KeyCheck(const float& fTimeDelta)
{
	int      iDistance = 0;

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
					m_iHighRenderState = MAGE_TRUN_LEFT;
				}
			}

			if (JUMP_CHECK && PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState && PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState
				&& PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState && PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState
				&& PLAYER_HEAL != m_pPlayerInfo->eMainState)
				m_iLowRenderState = MAGE_TRUN_LEFT;
		}
		m_pTransformCom->Rotation(CTransform::ANGLE_Y, D3DXToRadian(iDistance / 10.f));
	}

	Move_MouseAngle();

	if (m_pInputDev->OnceKeyDown(DIK_O))
	{
		if (m_pCollisionManager->Get_Render())
			m_pCollisionManager->Set_Render(false);
		else
			m_pCollisionManager->Set_Render(true);
	}

	// WASD Space
	if (PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState &&  // jsh 못 움직이는 조건
		PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState &&
		PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState &&
		PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState &&
		PLAYER_ATTACK_FOURTH != m_pPlayerInfo->eMainState &&
		PLAYER_JUMP_ATTACK_FOURTH != m_pPlayerInfo->eMainState &&
		PLAYER_JUMP_FALLING_ATTACK_FOURTH != m_pPlayerInfo->eMainState &&
		PLAYER_SKILL != m_pPlayerInfo->eMainState &&
		PLAYER_SKILL_ATTACK != m_pPlayerInfo->eMainState &&
		PLAYER_HEAL != m_pPlayerInfo->eMainState) // 못 움직이는 조건 jsh
	{
		// A (왼쪽)
		if (m_pInputDev->StayKeyDown(DIK_A))
		{
			if (ATTACK_HURT_CHECK && JUMP_CHECK)
				m_pPlayerInfo->eMainState = PLAYER_MOVE;
			if (JUMP_CHECK)
				m_iLowRenderState = MAGE_MOVE_LEFT;
			if (PLAYER_ATTACK_SECOND != m_pPlayerInfo->eMainState 
				&& PLAYER_ATTACK_THIRD != m_pPlayerInfo->eMainState
				&& PLAYER_JUMP_FALLING_ATTACK_SECOND != m_pPlayerInfo->eMainState
				&& PLAYER_JUMP_FALLING_ATTACK_THIRD != m_pPlayerInfo->eMainState
				&& JUMP_CHECK && PLAYER_HURT != m_pPlayerInfo->eMainState)
				m_iHighRenderState = MAGE_MOVE_LEFT;

			m_pPlayerInfo->tUnitInfo.fSpeedX = -50.f;
		}

		if (m_pInputDev->StayKeyDown(DIK_D))
		{
			if (ATTACK_HURT_CHECK && JUMP_CHECK)
				m_pPlayerInfo->eMainState = PLAYER_MOVE;
			if (JUMP_CHECK)
				m_iLowRenderState = MAGE_MOVE_RIGHT;
			if (PLAYER_ATTACK_SECOND != m_pPlayerInfo->eMainState
				&& PLAYER_ATTACK_THIRD != m_pPlayerInfo->eMainState
				&& PLAYER_JUMP_FALLING_ATTACK_SECOND != m_pPlayerInfo->eMainState
				&& PLAYER_JUMP_FALLING_ATTACK_THIRD != m_pPlayerInfo->eMainState
				&& JUMP_CHECK && PLAYER_HURT != m_pPlayerInfo->eMainState)
				m_iHighRenderState = MAGE_MOVE_RIGHT;

			m_pPlayerInfo->tUnitInfo.fSpeedX = 50.f;
		}

		if (m_pInputDev->StayKeyDown(DIK_W))
		{
			if (ATTACK_HURT_CHECK && JUMP_CHECK)
				m_pPlayerInfo->eMainState = PLAYER_MOVE;
			if (JUMP_CHECK)
				m_iLowRenderState = MAGE_RUN_FORWARD;
			if (PLAYER_ATTACK_SECOND != m_pPlayerInfo->eMainState
				&& PLAYER_ATTACK_THIRD != m_pPlayerInfo->eMainState
				&& PLAYER_JUMP_FALLING_ATTACK_SECOND != m_pPlayerInfo->eMainState
				&& PLAYER_JUMP_FALLING_ATTACK_THIRD != m_pPlayerInfo->eMainState
				&& JUMP_CHECK && PLAYER_HURT != m_pPlayerInfo->eMainState)
				m_iHighRenderState = MAGE_RUN_FORWARD;
			m_pPlayerInfo->tUnitInfo.fSpeedZ = 70.f;
		}
		else if (m_pInputDev->StayKeyDown(DIK_S))
		{
			if (ATTACK_HURT_CHECK && JUMP_CHECK)
				m_pPlayerInfo->eMainState = PLAYER_MOVE;
			if (JUMP_CHECK)
				m_iLowRenderState = MAGE_MOVE_BACKWARD;
			if (PLAYER_ATTACK_SECOND != m_pPlayerInfo->eMainState
				&& PLAYER_ATTACK_THIRD != m_pPlayerInfo->eMainState
				&& PLAYER_JUMP_FALLING_ATTACK_SECOND != m_pPlayerInfo->eMainState
				&& PLAYER_JUMP_FALLING_ATTACK_THIRD != m_pPlayerInfo->eMainState
				&& JUMP_CHECK && PLAYER_HURT != m_pPlayerInfo->eMainState)
				m_iHighRenderState = MAGE_MOVE_BACKWARD;
			m_pPlayerInfo->tUnitInfo.fSpeedZ = -70.f;
		}

		if (m_pInputDev->OnceKeyDown(DIK_SPACE))
		{
			if (ATTACK_HURT_CHECK)
			{
				m_pPlayerInfo->eMainState = PLAYER_JUMP;
			}
			if (m_pPlayerInfo->eMainState != PLAYER_JUMP &&
				m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING)
			{
				m_iLowRenderState = MAGE_JUMP;
				m_iHighRenderState = MAGE_JUMP;
			}
			CPlayer::Player_Jump(fTimeDelta);

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
	CPlayer::Control_PlayerMove(fTimeDelta);

	// 마우스 좌클릭
	if (m_pInputDev->OnceMouseClick(CInputDev::DIM_LBUTTON))
	{
		if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState) // 타워 미리보기 중
		{
			if (m_bAbleToBuild)
			{
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW_SECOND; // Preview2
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MONK, 0.5f);
			}
		}
		else if (PLAYER_TOWER_PREVIEW_SECOND == m_pPlayerInfo->eMainState)
		{
			m_pPlayerInfo->eMainState = PLAYER_TOWER_SUMMONING; // Summon 시작

			if (m_bMyClient)
				CSoundMgr::GetInstance()->Play_Sound(L"TowerSummoned2.ogg", Engine::CHANNEL_MAGE, 0.5f);
			if (m_bMyClient)
				CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MONK, 0.5f);
		}
		else
		{
			if (m_pPlayerInfo->eMainState != PLAYER_MOVE && ATTACK_HURT_CHECK && JUMP_CHECK && PLAYER_HEAL != m_pPlayerInfo->eMainState)
				m_iHighRenderState = HUNTRESS_FIRE;
			m_fAttackDelay =  0.f;
			if (ATTACK_CHECK)
			{
				if (PLAYER_JUMP == m_pPlayerInfo->eMainState)
					m_pPlayerInfo->eMainState = PLAYER_JUMP_ATTACK;
				else if (PLAYER_JUMP_FALLING == m_pPlayerInfo->eMainState)
					m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK;
				else
					m_pPlayerInfo->eMainState = PLAYER_ATTACK;
			}
		}
	}

	// 마우스 우클릭
	if (m_pInputDev->OnceMouseClick(CInputDev::DIM_RBUTTON))
	{
		// 타워 취소
		if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || 
			PLAYER_TOWER_PREVIEW_SECOND == m_pPlayerInfo->eMainState ||
			PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState) 
		{
			m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
		}
	}
	else if (m_pInputDev->StayMouseClick(CInputDev::DIM_RBUTTON))
	{
		if (PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState &&
			PLAYER_SKILL != m_pPlayerInfo->eMainState &&
			PLAYER_SKILL_ATTACK != m_pPlayerInfo->eMainState &&
			PLAYER_HEAL != m_pPlayerInfo->eMainState)
		{
			m_fGageCount += fTimeDelta;
			if (m_fGageCount >= 0.7f)
				m_fGageCount = 1.f;

			if (m_fGageCount < 1.f)
			{
				if (PLAYER_JUMP == m_pPlayerInfo->eMainState)
					m_pPlayerInfo->eMainState = PLAYER_JUMP_ATTACK_SECOND;
				else if (PLAYER_JUMP_FALLING == m_pPlayerInfo->eMainState)
					m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK_SECOND;
				else
					m_pPlayerInfo->eMainState = PLAYER_ATTACK_SECOND;
			}
			else
			{
				if (PLAYER_JUMP == m_pPlayerInfo->eMainState)
					m_pPlayerInfo->eMainState = PLAYER_JUMP_ATTACK_THIRD;
				else if (PLAYER_JUMP_FALLING == m_pPlayerInfo->eMainState)
					m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK_THIRD;
				else
					m_pPlayerInfo->eMainState = PLAYER_ATTACK_THIRD;
			}
		}
	}
	else
	{
		if (PLAYER_JUMP_ATTACK_THIRD == m_pPlayerInfo->eMainState)
			m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK_FOURTH;
		else if (PLAYER_JUMP_FALLING_ATTACK_THIRD == m_pPlayerInfo->eMainState)
			m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK_FOURTH;
		else if(PLAYER_ATTACK_THIRD == m_pPlayerInfo->eMainState)
			m_pPlayerInfo->eMainState = PLAYER_ATTACK_FOURTH;
		m_fGageCount = 0.f;
	}

	if (m_pInputDev->OnceKeyDown(DIK_1))
	{
		if (PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState &&
			PLAYER_HEAL != m_pPlayerInfo->eMainState)
		{
			if (m_pPlayerInfo->tUnitInfo.iMp > 0 
				&& m_pPlayerInfo->tUnitInfo.iHp < m_pPlayerInfo->tUnitInfo.iHpMax)	// Minkyeong
				m_pPlayerInfo->eMainState = PLAYER_HEAL;
			else if (m_bMyClient)
				CSoundMgr::GetInstance()->Play_Sound(L"MagicSpell_Invalid.ogg", Engine::CHANNEL_DARKELF, 0.4f);
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_3))
	{
		if (PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState &&
			PLAYER_HEAL != m_pPlayerInfo->eMainState &&
			PLAYER_SKILL != m_pPlayerInfo->eMainState)
		{
			if( 200 <= m_pPlayerInfo->tUnitInfo.iMp)
			{
				_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
				float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
				if (fDistance < 250.f)
				{
					CSoundMgr::GetInstance()->Play_Sound(L"MagicStaff_Charging.ogg", Engine::CHANNEL_MAGE_SKILL, 0.6f);
				}
				m_pPlayerInfo->eMainState = PLAYER_SKILL;
				m_pPlayerInfo->tUnitInfo.iMp -= 200;
			}
			else if (m_bMyClient)
				CSoundMgr::GetInstance()->Play_Sound(L"MagicSpell_Invalid.ogg", Engine::CHANNEL_DARKELF, 0.4f);
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_6))
	{
		if (m_pPlayerInfo->tUnitInfo.iMp >= 40)
		{
			if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState
				|| PLAYER_TOWER_PREVIEW_SECOND == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
			else if(ATTACK_HURT_CHECK)
			{
				// MONK_SUMMON
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW;
				m_eBuildTower = TOWER_TYPE_MISSILE;
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MONK, 0.5f);
			}
		}
		else if (m_bMyClient)
			CSoundMgr::GetInstance()->Play_Sound(L"MagicSpell_Invalid.ogg", Engine::CHANNEL_DARKELF, 0.4f);
	}

	if (m_pInputDev->OnceKeyDown(DIK_7))
	{
		if (m_pPlayerInfo->tUnitInfo.iMp >= 20)
		{
			if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState
				|| PLAYER_TOWER_PREVIEW_SECOND == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
			else if (ATTACK_HURT_CHECK)
			{
				// MONK_SUMMON
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW;
				m_eBuildTower = TOWER_TYPE_BLOCKADE;
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MONK, 0.5f);
			}
		}
		else if (m_bMyClient)
			CSoundMgr::GetInstance()->Play_Sound(L"MagicSpell_Invalid.ogg", Engine::CHANNEL_DARKELF, 0.4f);
	}

	if (m_pInputDev->OnceKeyDown(DIK_8))
	{
		if (m_pPlayerInfo->tUnitInfo.iMp >= 80)
		{
			if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState
				|| PLAYER_TOWER_PREVIEW_SECOND == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
			else if (ATTACK_HURT_CHECK)
			{
				// MONK_SUMMON
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW;
				m_eBuildTower = TOWER_TYPE_FIRE;
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MONK, 0.5f);
			}
		}
		else if (m_bMyClient)
			CSoundMgr::GetInstance()->Play_Sound(L"MagicSpell_Invalid.ogg", Engine::CHANNEL_DARKELF, 0.4f);
	}

	if (m_pInputDev->OnceKeyDown(DIK_9))
	{
		if (m_pPlayerInfo->tUnitInfo.iMp >= 120)
		{
			if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState
				|| PLAYER_TOWER_PREVIEW_SECOND == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
			else if (ATTACK_HURT_CHECK)
			{
				// MONK_SUMMON
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW;
				m_eBuildTower = TOWER_TYPE_LIGHTNING;
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MONK, 0.5f);
			}
		}
		else if (m_bMyClient)
			CSoundMgr::GetInstance()->Play_Sound(L"MagicSpell_Invalid.ogg", Engine::CHANNEL_DARKELF, 0.4f);
	}

	if (m_pInputDev->OnceKeyDown(DIK_0))
	{
		if (m_pPlayerInfo->tUnitInfo.iMp >= 150)
		{
			if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState || PLAYER_TOWER_SUMMONING == m_pPlayerInfo->eMainState
				|| PLAYER_TOWER_PREVIEW_SECOND == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;
			else if (ATTACK_HURT_CHECK)
			{
				// MONK_SUMMON
				m_pPlayerInfo->eMainState = PLAYER_TOWER_PREVIEW;
				m_eBuildTower = TOWER_TYPE_STRIKER;
				if (m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"button.wav", Engine::CHANNEL_MONK, 0.5f);
			}
		}
		else if (m_bMyClient)
			CSoundMgr::GetInstance()->Play_Sound(L"MagicSpell_Invalid.ogg", Engine::CHANNEL_DARKELF, 0.4f);
	}

	// 레디 박아
	if (m_pInputDev->OnceKeyDown(DIK_E))
	{
		// 크리스탈 위치를 원점으로 가정했음
		if (D3DXVec3Length(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION)) < 30.f)
		{
			// 서버로 준비되었다고 보내줌
			Ready_Combat();

			CSoundMgr::GetInstance()->Play_Sound(L"Crystal_Activate.ogg", Engine::CHANNEL_CRYSTAL_ACTIVATE, 0.8f);
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

	if (m_pInputDev->OnceKeyDown(DIK_INSERT))
	{
		_vec3 vecPick;
		
		if (FALSE == m_pCollisionManager->Pick_MouseCol(vecPick))
			return;

		CGameObject* pGameObject = CEasterEgg::Create(m_pGraphicDev);
		pGameObject->Set_Pos(&vecPick);
		CObject_Manager::GetInstance()->Add_GameObject(g_eCurrentScene, L"Layer_GameObject", pGameObject);

		CDataManager::GetInstance()->Add_EasterEgg(pGameObject, vecPick);
	}

	if (m_pInputDev->OnceKeyDown(DIK_DELETE))
	{
		CDataManager::GetInstance()->Delete_EasterEgg();
	}

	if (m_pInputDev->OnceKeyDown(DIK_F6))
	{
		_vec3 vecPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);

		CGameObject* pGameObject = CEasterEgg::Create(m_pGraphicDev);
		pGameObject->Set_Pos(&vecPos);
		CObject_Manager::GetInstance()->Add_GameObject(g_eCurrentScene, L"Layer_GameObject", pGameObject);

		CDataManager::GetInstance()->Add_EasterEgg(pGameObject, vecPos);
	}

	if (m_pInputDev->OnceKeyDown(DIK_SUBTRACT))
	{
		CCamera_Manager::GetInstance()->Change_Camera(CAMERA_ACTION, CUTSCENE_BOSS);

		//CCamera_Manager::GetInstance()->Shake_Camera(2000.f, 0.9f, 3); // 강도, 시간, 횟수
		//Hit_UI(0.1f);
	}

}

void CPlayer_Mage::KeyCheck_Death(void)
{
	if (m_pInputDev->OnceKeyDown(DIK_SPACE))
	{
		CCamera_Manager::GetInstance()->Change_Camera(CAMERA_DYNAMIC);
	}
}

void CPlayer_Mage::Set_Collision(void)
{
	if (m_iHighRenderState == MAGE_KNOCKBACK && m_iAttackCount == 0 && m_pDynamicMeshCom->GetTrackPosition() >= 0.8f && m_pDynamicMeshCom->GetTrackPosition() <= 0.9f)
	{
		m_iAttackCount += 2;

		if (m_bIsCharging&& m_Templist.size() >= 0)	// Minkyeong
		{
			list<Engine::CEffect*>::iterator iter_begin = m_Templist.begin();
			list<Engine::CEffect*>::iterator iter_end = m_Templist.end();
			for (; iter_begin != iter_end; ++iter_begin)
				(*iter_begin)->Set_Dead();
			m_Templist.clear();
			m_bIsCharging = FALSE;

			//////쉐이크 : 우클릭
			CCamera_Manager::GetInstance()->Shake_Camera(900.f, 1.f, 3); // 강도, 시간, 횟수

			///// 2번째 이펙트 /////
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Mage_Charging_Att", &m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION));

			_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
			float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
			if (fDistance < 350.f)
			{
				CSoundMgr::GetInstance()->StopSound(CHANNEL_MAGE_SKILL);
				CSoundMgr::GetInstance()->Play_Sound(L"present_expensive.ogg", Engine::CHANNEL_MAGE_SKILL2);
			}

			// Damage
			Engine::CGameObject*		pGameObject = nullptr;
			pGameObject = CSplashDamage::Create(m_pGraphicDev, m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION), MAGE_SPLASH_ATTACK);
			((CSplashDamage*)pGameObject)->Set_Damage(m_pPlayerInfo->tUnitInfo.iAttPower * 5);
			if (FAILED(Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
				return;
		}
	}

	// 좌클릭 평타
	if (PLAYER_JUMP_ATTACK == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK == m_pPlayerInfo->eMainState ||
		PLAYER_ATTACK == m_pPlayerInfo->eMainState)
	{
		if (PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState &&
			PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState &&
			PLAYER_HEAL != m_pPlayerInfo->eMainState &&
			m_fAttackDelay == 0.f)
		{
			m_fAttackDelay = 1.f;
			// 미사일 생성구간
			_vec3 vDir;

			if (m_bMyClient)
			{
				if (FALSE == m_pCollisionManager->Pick_MouseCol(vDir))
					return;
				
				m_vecPick = vDir;
			}
			else
				vDir = m_tLocalData.vecPick;
			
			m_pWeapon->AttackMissile(vDir, m_pPlayerInfo->tUnitInfo.iAttPower);
			_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
			float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
			if (fDistance < 200.f)
			{
				CSoundMgr::GetInstance()->Play_Sound(L"Impact_pawn_bullet1.ogg", Engine::CHANNEL_MAGE, 0.5f);
			}

			//////////////////////////////////////////////////////////////////////////
		}
	}

	if (m_iHighRenderState == MAGE_MANA_BOMB && m_iAttackCount == 0 && m_pDynamicMeshCom->GetTrackPosition() >= 1.0f && m_pDynamicMeshCom->GetTrackPosition() <= 1.2f)
	{
		m_iAttackCount += 2;

		// Mage 큰기술

		list<Engine::CEffect*>::iterator iter_begin = m_Templist.begin();
		list<Engine::CEffect*>::iterator iter_end = m_Templist.end();
		for (; iter_begin != iter_end; ++iter_begin)
			(*iter_begin)->Set_Dead();
		m_Templist.clear();
		m_bIsCharging = FALSE;

		//////쉐이크 : 3번

		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 350.f)
		{
			CSoundMgr::GetInstance()->StopSound(CHANNEL_MAGE_SKILL);
			CSoundMgr::GetInstance()->Play_Sound(L"Trap_ExplodingMine1.ogg", Engine::CHANNEL_MAGE_SKILL2);
		}

		///// 2번째 이펙트 /////
		Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Mage_ManaBomb_Att", &m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION));

		// Damage
		Engine::CGameObject*		pGameObject = nullptr;
		pGameObject = CSplashDamage::Create(m_pGraphicDev, m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION), MAGE_MANA_BOMB_SPLASH_ATTACK);
		((CSplashDamage*)pGameObject)->Set_Damage(m_pPlayerInfo->tUnitInfo.iAttPower * 20);
		if (FAILED(Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
			return;
	}

	// Mana Bomb casting 이펙트
	if (PLAYER_SKILL == m_pPlayerInfo->eMainState)
	{
		if (!m_bIsCharging)
		{
			m_Templist = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Mage_Casting_ManaBomb"
				, m_pWeapon->Get_EffectPointMatrix());
			m_bIsCharging = TRUE;
		}
	}

	// 우클릭 Charging 이펙트
	if (PLAYER_JUMP_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK_SECOND == m_pPlayerInfo->eMainState ||
		PLAYER_ATTACK_SECOND == m_pPlayerInfo->eMainState)
	{
		if (!m_bIsCharging)
		{
			m_Templist = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Mage_Charging", m_pWeapon->Get_EffectPointMatrix());
			m_bIsCharging = TRUE;
		}
	}

}

CPlayer_Mage* CPlayer_Mage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer_Mage* pInstance = new CPlayer_Mage(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPlayer_Mage Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPlayer_Mage::Free(void)
{
	CPlayer::Observer_RemoveData();
	Engine::Safe_Delete(m_pPlayerInfo);
	m_pCollisionManager->Delete_Component(L"Com_Col_Body", m_pCollisionBody);

	CPlayer::Free();
}