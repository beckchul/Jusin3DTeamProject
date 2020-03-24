#include "stdafx.h"
#include "Player_Huntress.h"
#include "Renderer.h"
#include "InfoSubject.h"
#include "CollisionManager.h"
#include "InputDev.h"
#include "NavMgr.h"

#include "Player_CrossBow.h"
#include "CrossBowArrow.h"
#include "Object_Manager.h"

#include "SoundMgr.h"
#include "EffectMgr.h"
#include "Camera_Manager.h"

CPlayer_Huntress::CPlayer_Huntress(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayer(pGraphicDev)
{
}

CPlayer_Huntress::~CPlayer_Huntress()
{
}

HRESULT CPlayer_Huntress::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &(_vec3(0.7f, 0.7f, 0.7f)));

	switch (g_eCurrentScene)
	{
	case SCENE_STAGE:
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-6.f, -12.f, -120.f)));
		break;
	case SCENE_STAGE2:
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-503.f, 120.f, 256.f)));
		break;
	}

	Load_AnimationInfo("../Resources/Data/AnimationData/HuntressData.xml");
	m_pWeaponMatrix = m_pDynamicMeshCom->Get_FrameMatrix("b_WristR");

	m_pWeapon = CPlayer_CrossBow::Create(m_pGraphicDev);
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon->Set_MatrixInfo(m_pTransformCom->Get_WorldMatrix(), m_pWeaponMatrix);


	//m_pHighFilterBone[0] = "b_Spine3";
	//m_pHighFilterBone[1] = ".";
	//m_pHighFilterBone[2] = ".";
	m_pHighFilterBone[0] = ".";
	m_pHighFilterBone[1] = ".";
	m_pHighFilterBone[2] = ".";
	/*m_pHighFilterBone[0] = "b_ClavicleL";
	m_pHighFilterBone[1] = "b_ClavicleR";
	m_pHighFilterBone[2] = "b_Neck";*/

	m_pLowFilterBone[0] = "b_Waist";
	m_pLowFilterBone[1] = "b_ThighL";
	m_pLowFilterBone[2] = "b_ThighR";

	/*m_pLowFilterBone[0] = ".";
	m_pLowFilterBone[1] = ".";
	m_pLowFilterBone[2] = ".";*/

	m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	return NOERROR;
}

int CPlayer_Huntress::FirstUpdate_GameObject(const float & fTimeDelta)
{
	if (TRUE == m_bFirstUpdate)
		return 0;

	m_bFirstUpdate = TRUE;

	m_pPlayerInfo = new PLAYER_INFO(m_iClinetNum, m_pTransformCom, m_pWeaponMatrix, PLAYER_HUNTRESS, PLAYER_END, 123, 1, 0.f, INFO(200, 200, 20, 40, 20, false, 0.f, 0.f, 0.f));
	CPlayer::Observer_AddData();

	return 0;
}

int CPlayer_Huntress::Update_GameObject(const float & fTimeDelta)
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
				m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-6.f, 20.f, 120.f)));
				m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
				m_pPlayerInfo->tUnitInfo.iHp = m_pPlayerInfo->tUnitInfo.iHpMax;
				m_pPlayerInfo->tUnitInfo.iMp = 0;

				Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Alive", &_vec3(-6.f, 15.f, 120.f));

				CCamera_Manager::GetInstance()->Change_Camera(CAMERA_STATIC, 0, -1.f);
			}
			break;
		case SCENE_STAGE2:
			return 1;
		}
	}

	m_fTimeDelta = fTimeDelta;
	m_fTrunCount -= fTimeDelta * 10.f;
	m_fAttackDelay -= fTimeDelta * 2.f;

	Change_State();

	if (m_iClinetNum == g_iClientIndex) // 내 플레이어
	{
		KeyCheck(fTimeDelta);

		CPlayer::Control_PlayerMove(fTimeDelta);

		CPlayer::Player_Fall(fTimeDelta); // 중력

		CPlayer::Player_Check_SecondFloor();

		CPlayer::Preview_Tower(fTimeDelta); // 타워 미리보기
		CPlayer::Summoning_Tower(fTimeDelta); // 타워 소환중... (끝나면 Build_Tower)

		if (-1 == m_dwIndex)
			m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

		Engine::CGameObject::Update_GameObject(fTimeDelta); // Transform Update

		SendLocalData(); // 자신의 정보(WorldMatrix)를 서버로 보낸다

		//cout << m_dwIndex << endl;
	}
	else // 다른 플레이어들
	{
		m_pTransformCom->Set_WorldMatrix(&m_tLocalData.matWorld);

		m_iHighRenderState = m_tLocalData.iHighAnimation;
		m_iLowRenderState = m_tLocalData.iLowAnimation;
		m_pPlayerInfo->eMainState = (PLAYER_MAIN_STATE)m_tLocalData.eMainState;
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DYNAMICMESH_NONEALPHA, this);

	CPlayer::Observer_Update();
	CPlayer::Check_Collision();
	Set_Collision();

	return 0;
}

void CPlayer_Huntress::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	CPlayer::Play_Animation(m_fTimeDelta);

	if (nullptr != m_pWeapon)
		m_pWeapon->Update_GameObject(m_fTimeDelta);

	m_pCollisionBody->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());

	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);

	m_bFirstFrame = true;
}

HRESULT CPlayer_Huntress::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*         pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_Huntress", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.CollisionSphere
	AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Body", 20.f, m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), 0.7f);
	m_pCollisionManager->Add_Component(L"Com_Col_Body", m_pCollisionBody);
	return NOERROR;
}

HRESULT CPlayer_Huntress::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pDynamicMeshCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

void CPlayer_Huntress::Change_State(void)
{
	if (PLAYER_HURT == m_pPlayerInfo->eMainState)
		m_iHighRenderState = HUNTERSS_HURT;
	else if (PLAYER_ATTACK == m_pPlayerInfo->eMainState)
		m_iHighRenderState = HUNTRESS_FIRE;
	else if (PLAYER_TURN == m_pPlayerInfo->eMainState)
		m_iHighRenderState = HUNTRESS_TURN_RIGHT;
	else if (PLAYER_JUMP == m_pPlayerInfo->eMainState)
		m_iHighRenderState = HUNTRESS_JUMP;
	else if (PLAYER_ATTACK_THIRD == m_pPlayerInfo->eMainState)
		m_iHighRenderState = HUNTRESS_RELOAD;
	else if (PLAYER_JUMP_FALLING == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK == m_pPlayerInfo->eMainState
		|| PLAYER_JUMP_FALLING_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK_THIRD == m_pPlayerInfo->eMainState
		|| PLAYER_JUMP_ATTACK == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK_THIRD == m_pPlayerInfo->eMainState)
		m_iHighRenderState = HUNTRESS_JUMP_FALLING;
	else
		m_iHighRenderState = HUNTRESS_IDLE;

	if (PLAYER_HURT == m_pPlayerInfo->eMainState)
		m_iLowRenderState = HUNTERSS_HURT;
	else if (PLAYER_JUMP == m_pPlayerInfo->eMainState)
		m_iLowRenderState = HUNTRESS_JUMP;
	else if (PLAYER_JUMP_FALLING == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK == m_pPlayerInfo->eMainState
		|| PLAYER_JUMP_FALLING_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK_THIRD == m_pPlayerInfo->eMainState
		|| PLAYER_JUMP_ATTACK == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK_THIRD == m_pPlayerInfo->eMainState)
		m_iLowRenderState = HUNTRESS_JUMP_FALLING;
	else
		m_iLowRenderState = HUNTRESS_IDLE;
}

void CPlayer_Huntress::KeyCheck(const float & fTimeDelta)
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
				m_pPlayerInfo->eMainState = PLAYER_TURN;
			}
			if (JUMP_CHECK)
			{
				m_iHighRenderState = HUNTRESS_TURN_RIGHT;
				m_iLowRenderState = HUNTRESS_TURN_RIGHT;
			}

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

	if (m_pInputDev->StayKeyDown(DIK_A))
	{
		if (ATTACK_HURT_CHECK && JUMP_CHECK)
			m_pPlayerInfo->eMainState = PLAYER_MOVE;
		if (JUMP_CHECK)
			m_iLowRenderState = HUNTRESS_RUN_LEFT;
		if (PLAYER_ATTACK_THIRD != m_pPlayerInfo->eMainState && JUMP_CHECK)
			m_iHighRenderState = HUNTRESS_RUN_LEFT;

		m_pPlayerInfo->tUnitInfo.fSpeedX = -45.f;
	}

	if (m_pInputDev->StayKeyDown(DIK_D))
	{
		if (ATTACK_HURT_CHECK && JUMP_CHECK)
			m_pPlayerInfo->eMainState = PLAYER_MOVE;
		if (JUMP_CHECK)
			m_iLowRenderState = HUNTRESS_RUN_RIGHT;
		if (PLAYER_ATTACK_THIRD != m_pPlayerInfo->eMainState && JUMP_CHECK)
			m_iHighRenderState = HUNTRESS_RUN_RIGHT;

		m_pPlayerInfo->tUnitInfo.fSpeedX = 45.f;
	}

	if (m_pInputDev->StayKeyDown(DIK_W))
	{
		if (ATTACK_HURT_CHECK && JUMP_CHECK)
			m_pPlayerInfo->eMainState = PLAYER_MOVE;
		if (JUMP_CHECK)
			m_iLowRenderState = HUNTRESS_RUN_FORWARD;
		if (PLAYER_ATTACK_THIRD != m_pPlayerInfo->eMainState && JUMP_CHECK)
			m_iHighRenderState = HUNTRESS_RUN_FORWARD;
		m_pPlayerInfo->tUnitInfo.fSpeedZ = 60.f;
	}
	else if (m_pInputDev->StayKeyDown(DIK_S))
	{
		if (ATTACK_HURT_CHECK && JUMP_CHECK)
			m_pPlayerInfo->eMainState = PLAYER_MOVE;
		if (JUMP_CHECK)
			m_iLowRenderState = HUNTRESS_RUN_BACKWARDS;
		if (PLAYER_ATTACK_THIRD != m_pPlayerInfo->eMainState && JUMP_CHECK)
			m_iHighRenderState = HUNTRESS_RUN_BACKWARDS;
		m_pPlayerInfo->tUnitInfo.fSpeedZ = -60.f;
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
			m_iLowRenderState = HUNTRESS_JUMP;
			m_iHighRenderState = HUNTRESS_JUMP;
		}
		CPlayer::Player_Jump(fTimeDelta);
	}

	if (m_pInputDev->OnceKeyDown(DIK_T))
	{
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(-6.f, 20.f, -120.f)));
		m_pPlayerInfo->eMainState = PLAYER_NOT_INPUT;
		m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	}

	if (m_pInputDev->OnceMouseClick(CInputDev::DIM_LBUTTON)) // 마우스 좌클릭
	{
		if (m_pPlayerInfo->eMainState != PLAYER_MOVE && ATTACK_HURT_CHECK)
			m_iHighRenderState = HUNTRESS_FIRE;
		if (ATTACK_CHECK)
		{
			if (PLAYER_JUMP == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_JUMP_ATTACK;
			else if (PLAYER_JUMP_FALLING == m_pPlayerInfo->eMainState)
				m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK;
			else
				m_pPlayerInfo->eMainState = PLAYER_ATTACK;
		}

		if (m_fAttackDelay <= 0.f)
		{
			m_fAttackDelay = 1.f;
			// 화살 생성구간 //////////////////////////////////////////////////////////////////////////


			_matrix* pMatHand = m_pDynamicMeshCom->Get_FrameMatrix("b_WristL");
			(*pMatHand) *= *(m_pTransformCom->Get_WorldMatrix());
			_vec3 vPosition;
			memcpy(&vPosition, &pMatHand->m[3][0], sizeof(_vec3));


			_vec3 vDir;
			m_pCollisionManager->Pick_MouseCol(vDir);
			D3DXVec3Normalize(&vDir, &(vDir - vPosition));


			_vec3 vAngle = m_pTransformCom->Get_Info(CTransform::INFO_ANGLE);
			vAngle.y += D3DXToRadian(-90.f);

			Engine::CGameObject*		pGameObject = nullptr;
			pGameObject = CCrossBow_Arrow::Create(m_pGraphicDev, vPosition, vDir, vAngle);
			if (nullptr == pGameObject)
				return;
			((CCrossBow_Arrow*)pGameObject)->Set_Damage(m_pPlayerInfo->tUnitInfo.iAttPower);
			((CCrossBow_Arrow*)pGameObject)->Set_Damage(m_pPlayerInfo->tUnitInfo.iAttPower);
			if (FAILED(Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
				return;

			//////////////////////////////////////////////////////////////////////////
		}

	}

	if (m_pInputDev->OnceMouseClick(CInputDev::DIM_RBUTTON)) // 마우스 우클릭
	{
		m_pPlayerInfo->eMainState = PLAYER_ATTACK_THIRD;
		m_iHighRenderState = HUNTRESS_RELOAD;
	}

	if (m_pInputDev->OnceKeyDown(DIK_NUMPADSLASH))
	{
		_vec3 vecPlayer = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);

		cout << "플레이어" << vecPlayer.x << " " << vecPlayer.y << " " << vecPlayer.z << endl;
	}

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
}

void CPlayer_Huntress::Set_Collision(void)
{
}

CPlayer_Huntress * CPlayer_Huntress::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer_Huntress* pInstance = new CPlayer_Huntress(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPlayer_Huntress Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPlayer_Huntress::Free(void)
{
	CPlayer::Observer_RemoveData();

	Engine::Safe_Delete(m_pPlayerInfo);
	m_pCollisionManager->Delete_Component(L"Com_Col_Body", m_pCollisionBody);
	CPlayer::Free();
}
