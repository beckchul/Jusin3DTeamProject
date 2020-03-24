#include "stdafx.h"
#include "Player_Goblin.h"
#include "Renderer.h"
#include "InfoSubject.h"
#include "NavMgr.h"
#include "Camera_Manager.h"
#include "ClientNetwork.h"
#include "FrustumManager.h"
#include "DataManager.h"
#include "InputDev.h"

#include "SoundMgr.h"
#include "DataManager.h"
#include "CollisionManager.h"

#include "EasterEgg.h"
#include "Player_ID.h"
#include "Object_Manager.h"
#include "EffectMgr.h"
#include "Effect_Decal.h"
#include "LastManObserver.h"

#include "EffectMgr.h"

USING(Engine)

_int CPlayer_Goblin::m_bMyClientDead = false;

CPlayer_Goblin::CPlayer_Goblin(LPDIRECT3DDEVICE9 pGraphicDev)
	: CDynamicObject(pGraphicDev)
	, m_iClinetNum(-1)
	, m_bMyClient(false)
	, m_bExitServer(false)
	, m_dwIndex(-1)
	, m_iPreRenderState(0)
	, m_pClinetNet(CClientNetwork::GetInstance())
	, m_pNavMgr(CNavMgr::GetInstance())
	, m_pInfoSubject(CInfoSubject::GetInstance())
	, m_fDeltaTime(0)
	, m_pLastManInfo(nullptr)
	, m_bFirstUpdate(false)
	, m_pInputDev(CInputDev::GetInstance())
	, m_pCollisionManager(CCollisionManager::GetInstance())
	, m_pPlayerID(nullptr)
	, m_bShowID(true)
	, m_pFrustumMgr(CFrustumManager::GetInstance())
	, m_fDelcalCoolDown(0.f)
	, m_pDataMgr(CDataManager::GetInstance())
{
	m_pFrustumMgr->Add_Ref();
	m_pClinetNet->Add_Ref();
	m_pNavMgr->Add_Ref();
	m_pDataMgr->Add_Ref();
	m_pInfoSubject->Add_Ref();
	m_pInputDev->Add_Ref();
	m_pCollisionManager->Add_Ref();
}

CPlayer_Goblin::~CPlayer_Goblin()
{
}

void CPlayer_Goblin::SetServerIndex(int iIndex)
{
	if (-1 == iIndex)
		cout << "인덱스 세팅이 제대로 안됐음!" << endl;

	m_iClinetNum = iIndex;
	cout << "m_iClinetNum" << m_iClinetNum << endl;

	if (m_iClinetNum == g_iClientIndex)
		m_bMyClient = true;
}

void CPlayer_Goblin::SendLocalData(void)
{
	Engine::NETDATA tData;
	ZeroMemory(&tData, sizeof(Engine::NETDATA));
	tData.chKey = 72;
	tData.eMessage = NETMSG_CLIENT_PLAYER_UPDATE2;
	tData.iIndex = m_iClinetNum;
	tData.matWorld = *(m_pTransformCom->Get_WorldMatrix());
	tData.eMainState = m_pLastManInfo->eMainState;
	tData.vecPick = m_pTransformCom->Get_Info(CTransform::INFO_ANGLE); // Angle로 쓰자 여기선

	m_pClinetNet->SendMsg(tData);
}

void CPlayer_Goblin::Player_Fall(const float & fTimeDelta)
{
	m_pLastManInfo->tUnitInfo.fSpeedY -= 4.f * fTimeDelta;

	m_pTransformCom->Go_Down(m_pLastManInfo->tUnitInfo.fSpeedY);

	bool bDropKill = false;
	if (TRUE == m_pNavMgr->FallOnNavMesh(m_pTransformCom, m_dwIndex, bDropKill)) // Collide with NavMesh
	{
		m_pLastManInfo->tUnitInfo.fSpeedY = 0.f;

		switch (m_pLastManInfo->eMainState)
		{
		case MONSTER_JUMP:
		case MONSTER_JUMP_FALLING:
			m_pLastManInfo->eMainState = MONSTER_IDLE;
			break;
		}
	}

	if (bDropKill) // 낙사
	{
		if (m_bMyClient)
			CCamera_Manager::GetInstance()->Shake_Camera(8000.f, 0.9f, 3); // 강도, 시간, 횟수

		m_pLastManInfo->eMainState = MONSTER_DEAD;
		m_pTransformCom->Go_Down(-30.f);
	}
}

void CPlayer_Goblin::Player_Check_SecondFloor(void)
{
	if (-1 == m_dwIndex)
		return;

	if (FALSE == m_pNavMgr->IsAbleToUpStairs(m_dwIndex)) // 1->2층 갈 수 있는 내비메쉬인지
		return;

	m_dwIndex = m_pNavMgr->FindSecondFloor(m_pTransformCom->Get_Info(CTransform::INFO_POSITION), m_dwIndex);
}

HRESULT CPlayer_Goblin::Ready_GameObject()
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Load_AnimationInfo("../Resources/Data/AnimationData/GoblinData.xml");

	m_pLastManObserver = CLastManObserver::Create();
	m_pInfoSubject->Subscribe_Observer(m_pLastManObserver);

	Random_Position();
	// NavMesh
	m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	TCHAR* szDecalName[] = { L"Com_Texture_Spray_WellPlayed", L"Com_Texture_Spray_Sorry",
		L"Com_Texture_Spray_Oops"
		, L"Com_Texture_Backchul", L"Com_Texture_BackchulConfidence"
		, L"Com_Texture_BackchulPain", L"Com_Texture_BackchulKaraoke", L"Com_Texture_BackchulTurtle"
		, L"Com_Texture_BurgerPuppy" }; // 굿

	for (int i = 0; i < sizeof(szDecalName) / sizeof(szDecalName[0]); ++i)
		m_vecDecalName.push_back(szDecalName[i]);

	return NOERROR;
}

void CPlayer_Goblin::Server_SetDead(void)
{
	if (MONSTER_DEAD == m_pLastManInfo->eMainState)
		return;

	m_pLastManInfo->eMainState = MONSTER_DEAD;

	Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterDeadPinkBlood", m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), m_pTransformCom->Get_WorldMatrix());

	_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
	float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
	if (fDistance < 300.f)
	{
		if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MONSTER_ATTACK))
			CSoundMgr::GetInstance()->Play_RandomSound(L"Goblin_death.ogg", Engine::CHANNEL_MONSTER_ATTACK, 6, 0.5f);
	}
}

void CPlayer_Goblin::Observer_AddData(void)
{
	m_pInfoSubject->Add_Data(MESSAGE_LASTMAN_INFO, m_pLastManInfo);
}

void CPlayer_Goblin::Observer_Update(void)
{
	m_pInfoSubject->Notify_Message(MESSAGE_LASTMAN_INFO);
}

void CPlayer_Goblin::Observer_RemoveData(void)
{
	m_pInfoSubject->Remove_Data(MESSAGE_LASTMAN_INFO, m_pLastManInfo);
}

int CPlayer_Goblin::FirstUpdate_GameObject(const float & fTimeDelta)
{
	if (TRUE == m_bFirstUpdate)
		return 0;

	m_bFirstUpdate = true;

	m_pLastManInfo = new LASTMAN_INFO(m_iClinetNum, _vec3(0.f, 0.f, 0.f), m_pTransformCom->Get_WorldMatrix(), 0.f, INFO(100, 100, 100, 100, 10, false, 0.f, 0.f, 0.f), MONSTER_IDLE, GOBLIN_IDLE);

	Observer_AddData();

	Calculate_Radius();

	m_pCollisionBody->Set_Damage(m_iClinetNum);

	return 0;
}

int CPlayer_Goblin::Update_GameObject(const float& fTimeDelta)
{
	FirstUpdate_GameObject(fTimeDelta);

	if (m_bExitServer) // 서버 종료
		return 1;

	m_fDeltaTime = fTimeDelta;
	m_fSpeedRun = 1.f;

	if (m_bMyClient) // 내 플레이어
	{
		if (FALSE == m_pLastManInfo->tUnitInfo.bDead &&
			MONSTER_DEAD != m_pLastManInfo->eMainState)
		{
			KeyCheck(fTimeDelta);
			Control_PlayerMove(fTimeDelta);
			Player_Fall(fTimeDelta);
			Player_Check_SecondFloor();
		}

		Engine::CGameObject::Update_GameObject(fTimeDelta); // Transform Update

		if (-1 == m_dwIndex)
			m_dwIndex = m_pNavMgr->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

		SendLocalData(); // 자신의 정보(WorldMatrix)를 서버로 보낸다

		Observer_Update(); // 문제시 아래로 빼자
	}
	else // 다른 플레이어들
	{
		m_pTransformCom->Set_WorldMatrix(&m_tLocalData.matWorld);

		_vec3 vecPos;
		memcpy(&vecPos, m_tLocalData.matWorld.m[3], sizeof(_vec3));
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vecPos);
		m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &m_tLocalData.vecPick);

		m_pLastManInfo->eMainState = (MONSTER_MAIN_STATE)m_tLocalData.eMainState;
	}

	if (nullptr != m_pRendererCom)
	{
		if (m_pFrustumMgr->IsSphereInFrustum(m_pTransformCom->Get_Info(CTransform::INFO_POSITION), m_fRadius))
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DYNAMICMESH_NONEALPHA, this);
	}

	Change_State();
	Check_Collision();
	Check_Magnetic();

	Create_ID();
	Show_ID(fTimeDelta); // ID 보여주기

	return 0;
}

void CPlayer_Goblin::FirstRender_GameObject(_bool bTrue)
{
	if (bTrue)
		return;

	if (m_pLastManInfo->tUnitInfo.bDead)
		return;

	if (m_iPreRenderState != m_pLastManInfo->iRenderState)
	{
		m_iPreRenderState = m_pLastManInfo->iRenderState;
		if (PLAYER_MOVE == m_pLastManInfo->eMainState)
			m_pLastManInfo->eMainState = MONSTER_IDLE;
		if (!m_tInfo.bDead && m_pLastManInfo->eMainState != MONSTER_JUMP_FALLING)
			m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[m_pLastManInfo->iRenderState]);
	}

	if (m_pDynamicMeshCom->GetPeriod() - m_vecAnimationInfo[m_pLastManInfo->iRenderState].dEndAniCount <= m_pDynamicMeshCom->GetTrackPosition())
	{
		switch (m_pLastManInfo->eMainState)
		{
		case MONSTER_JUMP:
		case MONSTER_JUMP_FALLING:
			m_iPreRenderState = -1;
			m_pLastManInfo->eMainState = MONSTER_JUMP_FALLING;
			break;
		case MONSTER_DEAD:
			m_pLastManInfo->tUnitInfo.bDead = true;
			m_fDeadTrackPosition = (_float)m_pDynamicMeshCom->GetPeriod();
			m_iDeadTrackIndex = m_pLastManInfo->iRenderState;
			m_pInfoSubject->Notify_Message(MESSAGE_LASTMAN_PLAYER_DEAD);

			if (m_bMyClient)
			{
				m_bMyClientDead = true;

				if(g_eLastMan_Progress != LASTMAN_ROUND_FINISH)
					CCamera_Manager::GetInstance()->Change_Camera(CAMERA_DYNAMIC); // 카메라 전환
			}
				
			break;
		default:
			m_iPreRenderState = -1;
			m_pLastManInfo->eMainState = MONSTER_IDLE;
			break;
		}
	}

	switch (m_pLastManInfo->eMainState)
	{
	case MONSTER_JUMP_FALLING:
		m_pDynamicMeshCom->Play_AnimationSet(0.f);
		break;
	case MONSTER_MOVE:
	case MONSTER_ATTACK:
		m_pDynamicMeshCom->Play_AnimationSet(m_fDeltaTime * 1.2f * m_fSlowDelay * m_fSpeedRun);
	default:
		m_pDynamicMeshCom->Play_AnimationSet(m_fDeltaTime * m_fSpeedRun);
		break;
	}

	m_pCollisionBody->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());
	m_pCollsionWeapon->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_Goblin::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	if (m_pLastManInfo->tUnitInfo.bDead)
	{
		m_pDynamicMeshCom->Set_TrackPosition(m_iDeadTrackIndex, m_fDeadTrackPosition);
		m_pDynamicMeshCom->Play_AnimationSet(0.f);
		m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);
	}
	else
		m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

void CPlayer_Goblin::ResetPlayer(void)
{
	m_iPreRenderState = -1;
	m_pLastManInfo->tUnitInfo.bDead = false;
	m_pLastManInfo->eMainState = MONSTER_IDLE;
	Random_Position();
	m_dwIndex = m_pNavMgr->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
	m_pCollisionBody->Set_CheckCollsion(true);
	m_pCollisionBody->Set_HitCheck(false);
	m_pCollsionWeapon->Set_CheckCollsion(false);
	m_pCollsionWeapon->Set_Life(-1.f);
}

void CPlayer_Goblin::Random_Position(void)
{
	vector<_vec3> vecPos;
	DWORD dwByte = 0;
	HANDLE hFile = CreateFile(L"../Resources/Data/Wave/Player.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	while (true)
	{
		_vec3 vPos;

		ReadFile(hFile, &vPos, sizeof(_vec3), &dwByte, NULL);

		if (dwByte == 0)
			break;

		vPos.y += 3.f;
		vecPos.push_back(vPos);
	}
	CloseHandle(hFile);
	_uint iCount = vecPos.size();

	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vecPos[rand() % iCount]);
	//m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vecPos[0]);
}

void CPlayer_Goblin::Change_State()
{
	switch (m_pLastManInfo->eMainState)
	{
	case MONSTER_IDLE:
		m_pLastManInfo->iRenderState = GOBLIN_IDLE;
		break;
	case MONSTER_MOVE:
		m_pLastManInfo->iRenderState = GOBLIN_MOVE_FORWARD;
		break;
	case MONSTER_ATTACK:
		m_pLastManInfo->iRenderState = GOBLIN_ATTACK_1;
		break;
	case MONSTER_JUMP:
		m_pLastManInfo->iRenderState = GOBLIN_JUMP;
		break;
	case MONSTER_JUMP_FALLING:
		m_pLastManInfo->iRenderState = GOBLIN_JUMP;
		break;
	case MONSTER_DEAD:
		m_pLastManInfo->iRenderState = GOBLIN_DEATH;
		break;
	case MONSTER_SORK:
		m_pLastManInfo->iRenderState = GOBLIN_COUGH;
		break;
	case MONSTER_HURT:
		m_pLastManInfo->iRenderState = GOBLIN_HURT;
		break;
	}

	int iKillingComputer = -1;
	if ((iKillingComputer = (_int)m_pCollsionWeapon->Get_Life()) != -1)
	{
		Engine::NETDATA tData;
		ZeroMemory(&tData, sizeof(Engine::NETDATA));
		tData.chKey = 72;
		tData.eMessage = NETMSG_CLINET_KILL;
		tData.iIndex = m_iClinetNum; // 죽인 플레이어
		tData.wSubMessage = 1; // AI를 죽임
		tData.iHighAnimation = iKillingComputer; // 죽임 당한 AI
		m_pClinetNet->SendMsg(tData);

		m_pCollsionWeapon->Set_Life(-1.f);
		m_fSlowReadyTime = 0.f;
		Create_Spotlight();
	}
	if (m_fSlowReadyTime < 0.4f)
	{
		m_fSlowReadyTime += m_fDeltaTime;
		if (m_fSlowReadyTime >= 0.4f)
			m_fSlowTime = 0.f;
	}

	if (m_fSlowTime < 1.f)
	{
		m_fSlowTime += m_fDeltaTime * 0.2f;
		m_Effectlist.clear();
		if (m_fSlowTime >= 1.f)
			m_fSlowDelay = 1.f;
		else
			m_fSlowDelay = 0.2f;
	}

	if (m_pCollisionBody->Get_HitCheck())
	{
		m_pCollisionBody->Set_HitCheck(false);
		m_pLastManInfo->eMainState = MONSTER_DEAD;
	}

}

void CPlayer_Goblin::KeyCheck(const float & fTimeDelta)
{
	if (m_pLastManInfo->eMainState != MONSTER_ATTACK &&
		m_pLastManInfo->eMainState != MONSTER_HURT &&
		m_pLastManInfo->eMainState != MONSTER_SORK &&
		m_fSlowDelay == 1.f)
	{
		if (m_pInputDev->StayKeyDown(DIK_LSHIFT))
		{
			m_fSpeedRun = 1.6f;
		}
	}

	if (m_pLastManInfo->eMainState != MONSTER_ATTACK &&
		m_pLastManInfo->eMainState != MONSTER_HURT &&
		m_pLastManInfo->eMainState != MONSTER_SORK)
	{
		_bool bCheck = false;
		if (m_pInputDev->StayKeyDown(DIK_A))
		{
			m_pTransformCom->Rotation(CTransform::ANGLE_Y, D3DXToRadian(-300.f * fTimeDelta));
		}
		if (m_pInputDev->StayKeyDown(DIK_D))
		{
			m_pTransformCom->Rotation(CTransform::ANGLE_Y, D3DXToRadian(300.f * fTimeDelta));
		}
		if (m_pInputDev->StayKeyDown(DIK_W))
		{
			if (1.f == m_fSpeedRun)
				m_pLastManInfo->tUnitInfo.fSpeedZ = 70.f;
			else
				m_pLastManInfo->tUnitInfo.fSpeedZ = 150.f;
			bCheck = true;

		}
		if (m_pInputDev->StayKeyDown(DIK_S))
		{

			if (m_pLastManInfo->tUnitInfo.fSpeedZ == 70.f ||
				m_pLastManInfo->tUnitInfo.fSpeedZ == 150.f)
				m_pLastManInfo->tUnitInfo.fSpeedZ = 0.f;
			else
				m_pLastManInfo->tUnitInfo.fSpeedZ = -70.f;

			bCheck = true;

		}
		if (bCheck && m_pLastManInfo->eMainState != MONSTER_JUMP &&
			m_pLastManInfo->eMainState != MONSTER_JUMP_FALLING)
			m_pLastManInfo->eMainState = MONSTER_MOVE;
	}

	if (m_pLastManInfo->eMainState != MONSTER_JUMP &&
		m_pLastManInfo->eMainState != MONSTER_JUMP_FALLING &&
		m_pLastManInfo->eMainState != MONSTER_SORK)
	{
		if (m_pInputDev->OnceKeyDown(DIK_SPACE) && m_fSlowDelay == 1.f)
		{
			m_pLastManInfo->tUnitInfo.fSpeedY = 2.f;
			m_pLastManInfo->eMainState = MONSTER_JUMP;
		}

		if (g_eLastMan_Progress != LASTMAN_ROUND_WAIT) // 라운드 중에만
		{
			if (m_pInputDev->OnceMouseClick(CInputDev::DIM_LBUTTON) && m_fSlowDelay == 1.f)
			{
				m_pLastManInfo->eMainState = MONSTER_ATTACK;
			}
			if (m_pInputDev->OnceMouseClick(CInputDev::DIM_RBUTTON) && m_fSlowDelay == 1.f)
			{
				m_pLastManInfo->eMainState = MONSTER_HURT;
			}
		}
		
	}

	if (m_pInputDev->OnceKeyDown(DIK_G))
		m_pLastManInfo->eMainState = MONSTER_SORK;

	if (m_pInputDev->OnceKeyDown(DIK_DELETE))
		m_bShowID = (m_bShowID == TRUE) ? FALSE : TRUE;

	m_fDelcalCoolDown -= fTimeDelta;
	if (m_pInputDev->OnceKeyDown(DIK_1))
	{
		if (m_fDelcalCoolDown < 0.f)
		{
			m_fDelcalCoolDown = 1.f;

			Create_Decal(0);
			Send_Decal(0);
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_2))
	{
		if (m_fDelcalCoolDown < 0.f)
		{
			m_fDelcalCoolDown = 1.f;

			Create_Decal(1);
			Send_Decal(1);
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_3))
	{
		if (m_fDelcalCoolDown < 0.f)
		{
			m_fDelcalCoolDown = 1.f;

			Create_Decal(2);
			Send_Decal(2);
		}
	}

	if (m_pInputDev->OnceKeyDown(DIK_4))
	{
		if (m_fDelcalCoolDown < 0.f)
		{
			m_fDelcalCoolDown = 1.f;

			_uint iRand = Random(6);
			iRand += 2;
			while (iRand < 3)
				++iRand;

			Create_Decal(iRand);
			Send_Decal(iRand);
		}
	}

}

void CPlayer_Goblin::Control_PlayerMove(const float & fTimeDelta)
{
	if (0.f != m_pLastManInfo->tUnitInfo.fSpeedZ && 0.f == m_pLastManInfo->tUnitInfo.fSpeedX)
		m_dwIndex = m_pTransformCom->Go_Straight(fTimeDelta * m_pLastManInfo->tUnitInfo.fSpeedZ * m_fSlowDelay, m_dwIndex);
	if (0.f != m_pLastManInfo->tUnitInfo.fSpeedX && 0.f == m_pLastManInfo->tUnitInfo.fSpeedZ)
		m_dwIndex = m_pTransformCom->Go_Right(fTimeDelta * m_pLastManInfo->tUnitInfo.fSpeedX * m_fSlowDelay, m_dwIndex);
	if (0.f != m_pLastManInfo->tUnitInfo.fSpeedX && 0.f != m_pLastManInfo->tUnitInfo.fSpeedZ)
	{
		m_dwIndex = m_pTransformCom->Go_Straight(fTimeDelta * m_pLastManInfo->tUnitInfo.fSpeedZ * sqrtf(0.9f) * m_fSlowDelay, m_dwIndex);
		m_dwIndex = m_pTransformCom->Go_Right(fTimeDelta * m_pLastManInfo->tUnitInfo.fSpeedX * sqrtf(0.9f) * m_fSlowDelay, m_dwIndex);
	}

	if (3.f < m_pLastManInfo->tUnitInfo.fSpeedX)
		m_pLastManInfo->tUnitInfo.fSpeedX -= fTimeDelta * 200.f;
	else if (-3.f > m_pLastManInfo->tUnitInfo.fSpeedX)
		m_pLastManInfo->tUnitInfo.fSpeedX += fTimeDelta * 200.f;
	else
		m_pLastManInfo->tUnitInfo.fSpeedX = 0.f;

	if (3.f < m_pLastManInfo->tUnitInfo.fSpeedZ)
		m_pLastManInfo->tUnitInfo.fSpeedZ -= fTimeDelta * 200.f;
	else if (-3.f > m_pLastManInfo->tUnitInfo.fSpeedZ)
		m_pLastManInfo->tUnitInfo.fSpeedZ += fTimeDelta * 200.f;
	else
		m_pLastManInfo->tUnitInfo.fSpeedZ = 0.f;
}

void CPlayer_Goblin::Check_Collision(void)
{
	if (m_pLastManInfo->eMainState == MONSTER_HURT ||
		m_pLastManInfo->eMainState == MONSTER_DEAD ||
		m_pLastManInfo->tUnitInfo.bDead)
		return;

	auto pCollisionlist2 = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonBody");
	for (auto iter = pCollisionlist2.begin(); iter != pCollisionlist2.end(); ++iter)
	{
		if ((*iter) == m_pCollisionBody)
			continue;

		if (m_pCollsionWeapon->Get_CheckCollision()
			&& !((CCollisionSphere*)(*iter))->Get_HitCheck()
			&& m_pCollsionWeapon->Check_Collision((CCollisionSphere*)(*iter)))
		{
			((CCollisionSphere*)(*iter))->Set_HitCheck(true);
			((CCollisionSphere*)(*iter))->Set_CheckCollsion(false);
			//cout << ((CCollisionSphere*)(*iter))->Get_Damage() << "번 클라 죽임" << endl;

			Engine::NETDATA tData;
			ZeroMemory(&tData, sizeof(Engine::NETDATA));
			tData.chKey = 72;
			tData.eMessage = NETMSG_CLINET_KILL;
			tData.iIndex = m_iClinetNum; // 죽인 플레이어
			tData.wSubMessage = 0; // 플레이어를 죽임
			tData.iHighAnimation = ((CCollisionSphere*)(*iter))->Get_Damage(); // 죽임 당한 플레이어
			m_pClinetNet->SendMsg(tData);

			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterDeadPinkBlood", m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), m_pTransformCom->Get_WorldMatrix());

			_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
			float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
			if (fDistance < 300.f)
			{
				if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MONSTER_ATTACK))
					CSoundMgr::GetInstance()->Play_RandomSound(L"Goblin_death.ogg", Engine::CHANNEL_MONSTER_ATTACK, 6, 0.5f);
			}
		}
	}

	if (m_pLastManInfo->iRenderState == GOBLIN_ATTACK_1 && m_pDynamicMeshCom->GetTrackPosition() >= 0.5f && m_pDynamicMeshCom->GetTrackPosition() <= 0.7f)
	{
		m_pCollsionWeapon->Set_CheckCollsion(true);
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 250.f)
		{
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MONSTER_ATTACK))
				CSoundMgr::GetInstance()->Play_RandomSound(L"Goblin_attack.ogg", Engine::CHANNEL_MONSTER_ATTACK, 5, 0.5f);

			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MAGE))
				CSoundMgr::GetInstance()->Play_Sound(L"fluff2.mp3", Engine::CHANNEL_MAGE, 0.7f);


		}
	}
	else
		m_pCollsionWeapon->Set_CheckCollsion(false);
}

void CPlayer_Goblin::Create_ID(void)
{
	if (nullptr != m_pPlayerID)
		return;

	string s = CDataManager::GetInstance()->Get_Player_ID(m_iClinetNum);
	if (s == "")
		return;

	m_pPlayerID = CPlayer_ID::Create(m_pGraphicDev, m_iClinetNum);
	if (nullptr == m_pPlayerID)
		return;
}

void CPlayer_Goblin::Show_ID(const float & fTimeDelta)
{
	if (nullptr == m_pPlayerID) // 자신의 ID
		return;

	if (m_pLastManInfo->tUnitInfo.bDead || 
		(m_bMyClient && m_bShowID) || 
		g_eLastMan_Progress == LASTMAN_ROUND_WAIT ||
		g_eLastMan_Progress == LASTMAN_ROUND_FINISH ||
		m_bMyClientDead) // 내 클라이언트가 죽었을 때 나머지 다 보여줌
	{
		m_pPlayerID->Set_Pos(&(m_pTransformCom->Get_Info(CTransform::INFO_POSITION) + _vec3(0.f, 26.f, 0.f)));
		m_pPlayerID->Update_GameObject(fTimeDelta);
	}
}

void CPlayer_Goblin::Calculate_Radius(void)
{
	// 반지름 구하기
	_vec3 vecMin, vecMax, vecScale;
	m_pDynamicMeshCom->Compute_MinMax(&vecMin, &vecMax, 0);

	vecScale = m_pTransformCom->Get_Info(CTransform::INFO_SCALE);

	vecMin.x *= vecScale.x;
	vecMin.y *= vecScale.y;
	vecMin.z *= vecScale.z;

	vecMax.x *= vecScale.x;
	vecMax.y *= vecScale.y;
	vecMax.z *= vecScale.z;

	m_fRadius = D3DXVec3Length(&(vecMax - vecMin)) / 2.f;
}

void CPlayer_Goblin::Create_Spotlight(void)
{
	m_Effectlist = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Like_Spotlight", m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_Goblin::Check_Magnetic(void)
{
	if (LASTMAN_ROUND_ELECTRIC_ACTIVATE != g_eLastMan_Progress)
		return;

	_vec3 vecMagnetic = m_pDataMgr->Get_MagneticInitPos(g_iRound - 1); // 자기장 중심
	_float fMagRad = m_pDataMgr->Get_MagneticRadius(); // 자기장 반지름

	_vec3 vecDir = m_pTransformCom->Get_Info(CTransform::INFO_POSITION) - vecMagnetic;

	if (D3DXVec3Length(&vecDir) > fMagRad)
		Server_SetDead();
}

void CPlayer_Goblin::Create_Decal(WORD iDecalNum)
{
	if (m_vecDecalName.size() <= iDecalNum)
		return;

	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	_vec3 vAngle = m_pTransformCom->Get_Info(CTransform::INFO_ANGLE);

	CEffect_Decal* pEffect = CEffect_Decal::Create(m_pGraphicDev, m_vecDecalName[iDecalNum]);
	if (nullptr == pEffect)
		return;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(CTransform::INFO_POSITION, &_vec3(vPos.x, vPos.y, vPos.z));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(50.f, 13.f, 20.f));

	switch(iDecalNum)
	{
	case 3:	// 경복궁 사진
		pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 15.f, 30.f));
		break;
	case 4:	// 자신감? 사진
		pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(28.f, 15.f, 40.f));
		break;
	case 5:	// 아파하는 사진
		pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(40.f, 15.f, 40.f));
		break;
	case 6:	// 노래방 사진
		pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(47.f, 15.f, 40.f));
		break;
	case 7:	// 거북이 사진
		pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 15.f, 30.f));
		break;
	}
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, vAngle.y + D3DXToRadian(180.f), 0.f));
	dynamic_cast<CEffect_Decal*>(pEffect)->Set_DecalOp(1.f, 0.05f);

	Engine::CEffectMgr::GetInstance()->Add_Effect(pEffect);
	_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
	float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
	if (fDistance < 250.f)
	{
		CSoundMgr::GetInstance()->StopSound(Engine::CHANNEL_ORC);
		CSoundMgr::GetInstance()->Play_Sound(L"sp.mp3", Engine::CHANNEL_ORC, 0.5f);
	}
}

void CPlayer_Goblin::Send_Decal(WORD iDecalNum)
{
	Engine::NETDATA tData;
	ZeroMemory(&tData, sizeof(Engine::NETDATA));
	tData.chKey = 72;
	tData.eMessage = NETMSG_CLIENT_DECAL;
	tData.iIndex = m_iClinetNum;
	tData.wSubMessage = iDecalNum;

	m_pClinetNet->SendMsg(tData);
}

HRESULT CPlayer_Goblin::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_Goblin", Engine::CComponent::COM_STATIC, L"Com_Mesh");
	AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_MonBody", 10.f, m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), 1.f);
	AddCollisionSphere(m_pCollsionWeapon, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_MonWeapon", 9.f, m_pDynamicMeshCom->Get_FrameMatrix("b_FingersR"), 1.f);
	m_pCollisionManager->Add_Component(L"Com_Col_MonBody", m_pCollisionBody);
	m_pCollisionManager->Add_Component(L"Com_Col_MonWeapon", m_pCollsionWeapon);
	m_pCollisionBody->Set_CheckCollsion(true);
	m_pCollisionBody->Set_HitCheck(false);
	m_pCollsionWeapon->Set_CheckCollsion(false);
	m_pCollsionWeapon->Set_Life(-1.f);
	return NOERROR;
}

HRESULT CPlayer_Goblin::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CPlayer_Goblin* CPlayer_Goblin::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer_Goblin* pInstance = new CPlayer_Goblin(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPlayer_Goblin Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPlayer_Goblin::Free(void)
{
	Engine::Safe_Release(m_pClinetNet);
	Engine::Safe_Release(m_pNavMgr);

	m_pInfoSubject->UnSubscribe_Observer(m_pLastManObserver);
	Safe_Release(m_pLastManObserver);
	Engine::Safe_Release(m_pInfoSubject);
	Engine::Safe_Release(m_pDataMgr);

	Engine::Safe_Release(m_pInputDev);
	Engine::Safe_Release(m_pFrustumMgr);
	Engine::Safe_Release(m_pCollisionManager);
	m_pCollisionManager->Delete_Component(L"Com_Col_MonBody", m_pCollisionBody);
	m_pCollisionManager->Delete_Component(L"Com_Col_MonWeapon", m_pCollsionWeapon);
	Engine::Safe_Release(m_pCollisionBody);
	Engine::Safe_Release(m_pCollsionWeapon);

	Observer_RemoveData();

	Engine::Safe_Release(m_pPlayerID);
	Engine::Safe_Delete(m_pLastManInfo);

	CDynamicObject::Free();
}
