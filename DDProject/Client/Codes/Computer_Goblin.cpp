#include "stdafx.h"
#include "Computer_Goblin.h"
#include "Renderer.h"
#include "InfoSubject.h"
#include "NavMgr.h"
#include "Camera_Manager.h"
#include "ClientNetwork.h"
#include "FrustumManager.h"
#include "InputDev.h"
#include "DataManager.h"

#include "SoundMgr.h"
#include "DataManager.h"
#include "CollisionManager.h"

#include "LastManObserver.h"

#include "EffectMgr.h"

CComputer_Goblin::CComputer_Goblin(LPDIRECT3DDEVICE9 pGraphicDev, _uint uIndex)
	: CDynamicObject(pGraphicDev)
	, m_dwIndex(-1)
	, m_iRenderState(0)
	, m_iPreRenderState(0)
	, m_pNavMgr(CNavMgr::GetInstance())
	, m_fDeltaTime(0)
	, m_bFirstUpdate(false)
	, m_pInfoSubject(CInfoSubject::GetInstance())
	, m_pCollisionManager(CCollisionManager::GetInstance())
	, m_pFrustumMgr(CFrustumManager::GetInstance())
	, m_pDataMgr(CDataManager::GetInstance())
{
	m_pFrustumMgr->Add_Ref();
	m_pNavMgr->Add_Ref();
	m_pCollisionManager->Add_Ref();
	m_pInfoSubject->Add_Ref();
	m_tLastManInfo.iPlayerIdx = uIndex;
	m_pDataMgr->Add_Ref();
}

CComputer_Goblin::~CComputer_Goblin()
{
}

HRESULT CComputer_Goblin::Ready_GameObject(_vec3 vPosition, vector<_int> vecRamdom)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Load_AnimationInfo("../Resources/Data/AnimationData/GoblinData.xml");
	m_vecResetPosition = vPosition;
	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(0.f), 0.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPosition);
	m_pLastManObserver = CLastManObserver::Create();
	m_pInfoSubject->Subscribe_Observer(m_pLastManObserver);

	// NavMesh
	m_dwIndex = m_pNavMgr->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
	m_tLastManInfo = LASTMAN_INFO(m_tLastManInfo.iPlayerIdx, _vec3(0.f, 0.f, 0.f), m_pTransformCom->Get_WorldMatrix(), 0.f, INFO(100, 100, 100, 100, 10, false, 0.f, 0.f, 0.f), MONSTER_IDLE, GOBLIN_IDLE);
	m_vecRamdom = vecRamdom;
	return NOERROR;
}

void CComputer_Goblin::Server_SetDead(void)
{
	if (MONSTER_DEAD != m_tLastManInfo.eMainState)
	{
		m_tLastManInfo.eMainState = MONSTER_DEAD;
		Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterDeadGreenBlood", m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), m_pTransformCom->Get_WorldMatrix());
	}
}
void CComputer_Goblin::Player_Fall(const float & fTimeDelta)
{
	m_tLastManInfo.tUnitInfo.fSpeedY -= 4.f * fTimeDelta;

	m_pTransformCom->Go_Down(m_tLastManInfo.tUnitInfo.fSpeedY);

	bool bDropKill = false;
	if (TRUE == m_pNavMgr->FallOnNavMesh(m_pTransformCom, m_dwIndex, bDropKill)) // Collide with NavMesh
	{
		m_tLastManInfo.tUnitInfo.fSpeedY = 0.f;

		switch (m_tLastManInfo.eMainState)
		{
		case MONSTER_JUMP:
		case MONSTER_JUMP_FALLING:
			m_tLastManInfo.eMainState = MONSTER_IDLE;
			break;
		}
	}

	if (bDropKill) // 낙사
	{
		m_tLastManInfo.eMainState = MONSTER_DEAD;
	}
}

void CComputer_Goblin::Player_Check_SecondFloor(void)
{
}

int CComputer_Goblin::FirstUpdate_GameObject(const float & fTimeDelta)
{
	if (TRUE == m_bFirstUpdate)
		return 0;

	m_bFirstUpdate = true;

	Calculate_Radius();

	return 0;
}

int CComputer_Goblin::Update_GameObject(const float & fTimeDelta)
{
	FirstUpdate_GameObject(fTimeDelta);

	m_fDeltaTime = fTimeDelta;

	if (FALSE == m_tLastManInfo.tUnitInfo.bDead)
	{
		Pattern();
		Control_PlayerMove(fTimeDelta);
		Player_Fall(fTimeDelta);
	}

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if (m_pFrustumMgr->IsSphereInFrustum(m_pTransformCom->Get_Info(CTransform::INFO_POSITION), m_fRadius))
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DYNAMICMESH_NONEALPHA, this);
	}

	
	Check_Collision();
	Check_Magnetic();
	Change_State();
	return 0;
}

void CComputer_Goblin::FirstRender_GameObject(_bool bTrue)
{
	if (bTrue)
		return;

	if (m_tLastManInfo.tUnitInfo.bDead)
		return;

	if (m_iPreRenderState != m_iRenderState)
	{
		m_iPreRenderState = m_iRenderState;
		if (PLAYER_MOVE == m_tLastManInfo.eMainState)
			m_tLastManInfo.eMainState = MONSTER_IDLE;
		if (!m_tInfo.bDead && m_tLastManInfo.eMainState != MONSTER_JUMP_FALLING)
			m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[m_iRenderState]);
	}

	if (m_pDynamicMeshCom->GetPeriod() - m_vecAnimationInfo[m_iRenderState].dEndAniCount <= m_pDynamicMeshCom->GetTrackPosition())
	{
		switch (m_tLastManInfo.eMainState)
		{
		case MONSTER_JUMP:
		case MONSTER_JUMP_FALLING:
			m_iPreRenderState = -1;
			m_tLastManInfo.eMainState = MONSTER_JUMP_FALLING;
			break;
		case MONSTER_DEAD:
			m_tLastManInfo.tUnitInfo.bDead = true;
			m_fDeadTrackPosition = (_float)m_pDynamicMeshCom->GetPeriod();
			m_iDeadTrackIndex = m_iRenderState;
			m_pInfoSubject->Notify_Message(MESSAGE_LASTMAN_COMPUTER_DEAD);
			break;
		default:
			m_iPreRenderState = -1;
			m_tLastManInfo.eMainState = MONSTER_IDLE;
			break;
		}
	}

	switch (m_tLastManInfo.eMainState)
	{
	case MONSTER_JUMP_FALLING:
		m_pDynamicMeshCom->Play_AnimationSet(0.f);
		break;
	case MONSTER_MOVE:
	case MONSTER_ATTACK:
		m_pDynamicMeshCom->Play_AnimationSet(m_fDeltaTime * 1.2f);
	default:
		m_pDynamicMeshCom->Play_AnimationSet(m_fDeltaTime);
		break;
	}

	m_pCollisionBody->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());
}

void CComputer_Goblin::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	if (m_tLastManInfo.tUnitInfo.bDead)
	{
		m_pDynamicMeshCom->Set_TrackPosition(m_iDeadTrackIndex, m_fDeadTrackPosition);
		m_pDynamicMeshCom->Play_AnimationSet(0.f);
		m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);
	}
	else
		m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);

	m_bFirstRender = true;
}


void CComputer_Goblin::Reset_AI(void)
{
	m_iRamdom = 0;
	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(0.f), 0.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &m_vecResetPosition);
	m_dwIndex = m_pNavMgr->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_tLastManInfo.eMainState = MONSTER_IDLE;
	m_tLastManInfo.tUnitInfo.bDead = false;
	m_fPattonTime = 0.f;
	m_fCount = 0.f;
	m_iPattonCount = 0;
}

void CComputer_Goblin::Pattern(void)
{
	if (m_tLastManInfo.eMainState == MONSTER_DEAD)
		return;

	if (m_fPattonTime > 0)
	{
		m_fPattonTime -= m_fDeltaTime;
		if (MONSTER_IDLE != m_ePattonState)
		{
			m_tLastManInfo.tUnitInfo.fSpeedZ = m_fZMove;
			m_tLastManInfo.tUnitInfo.fSpeedX = m_fXMove;
		}
		if (MONSTER_JUMP != m_ePattonState)
			m_tLastManInfo.eMainState = m_ePattonState;

		m_pTransformCom->Rotation(CTransform::ANGLE_Y, m_fDeltaTime * D3DXToRadian(m_fRotationCount));
		return;
	}
	else
	{
		m_ePattonState = MONSTER_IDLE;
		m_fRotationCount = 0;
	}

	m_fCount += m_fDeltaTime;
	if (m_fCount >= 1.f)
	{
		m_fCount = 0.f;
		++m_iPattonCount;
	}

	if ((m_iPattonCount % 2) == 0 && 0 == m_fCount)
	{
		m_iRandomPatton = m_vecRamdom[m_iRamdom];
		++m_iRamdom;
		if (m_iRamdom >= 399)
			m_iRamdom = 0;

		switch (m_iRandomPatton)
		{
		case 0:
			m_fRotationCount = 300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 1.f;
			break;
		case 1:
			m_fRotationCount = -300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 1.f;
			break;
		case 2:
			m_fRotationCount = 300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 1.5f;
			break;
		case 3:
			m_fRotationCount = -300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 1.5f;
			break;
		case 4:
			m_fRotationCount = 300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 0.5f;
			break;
		case 5:
			m_fRotationCount = -300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 0.5f;
			break;
		case 6:
			m_fRotationCount = 300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 0.7f;
			break;
		case 7:
			m_fRotationCount = -300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 0.7f;
			break;
		case 8:
			m_fRotationCount = 300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 0.9f;
			break;
		case 9:
			m_fRotationCount = -300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 0.9f;
			break;
		case 10:
			m_fRotationCount = 300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 1.2f;
			break;
		case 11:
			m_fRotationCount = -300.f;
			m_ePattonState = MONSTER_IDLE;
			m_fPattonTime = 1.2f;
			break;
		case 12:
			m_fZMove = 70.f;
			m_fXMove = 0.f;
			m_ePattonState = MONSTER_MOVE;
			m_fPattonTime = 1.2f;
			break;
		case 13:
			m_fZMove = -70.f;
			m_fXMove = 0.f;
			m_ePattonState = MONSTER_MOVE;
			m_fPattonTime = 1.2f;
			break;
		case 14:
			m_fZMove = 70.f;
			m_fXMove = 0.f;
			m_ePattonState = MONSTER_MOVE;
			m_fPattonTime = 0.4f;
			break;
		case 15:
			m_fZMove = -70.f;
			m_fXMove = 0.f;
			m_ePattonState = MONSTER_MOVE;
			m_fPattonTime = 0.4f;
			break;
		case 16:
			m_fZMove = 70.f;
			m_fXMove = 0.f;
			m_ePattonState = MONSTER_MOVE;
			m_fPattonTime = 0.8f;
			break;
		case 17:
			m_fZMove = -70.f;
			m_fXMove = 0.f;
			m_ePattonState = MONSTER_MOVE;
			m_fPattonTime = 0.8f;
			break;
		case 18:
			m_fZMove = 70.f;
			m_fPattonTime = 0.9f;
			m_tLastManInfo.tUnitInfo.fSpeedY = 2.f;
			m_ePattonState = MONSTER_JUMP;
			m_tLastManInfo.eMainState = MONSTER_JUMP;
			break;
		case 19:
			m_fPattonTime = 0.9f;
			m_tLastManInfo.tUnitInfo.fSpeedY = 2.f;
			m_ePattonState = MONSTER_JUMP;
			m_tLastManInfo.eMainState = MONSTER_JUMP;
			m_fZMove = 0.f;
			break;
		case 20:
			m_fZMove = 0.f;
			m_fPattonTime = 0.5f;
			m_ePattonState = MONSTER_ATTACK;
			m_tLastManInfo.eMainState = MONSTER_ATTACK;
			break;
		case 21:
			m_fPattonTime = 0.9f;
			m_fRotationCount = 100;
			m_fZMove = 70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 22:
			m_fPattonTime = 0.9f;
			m_fRotationCount = -100;
			m_fZMove = 70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 23:
			m_fPattonTime = 1.4f;
			m_fRotationCount = 100;
			m_fZMove = 70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 24:
			m_fPattonTime = 1.4f;
			m_fRotationCount = -100;
			m_fZMove = 70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 25:
			m_fPattonTime = 0.7f;
			m_fRotationCount = 200;
			m_fZMove = 70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 26:
			m_fPattonTime = 0.7f;
			m_fRotationCount = -200;
			m_fZMove = 70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 27:
			m_fPattonTime = 0.5f;
			m_fRotationCount = 50;
			m_fZMove = 70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 28:
			m_fPattonTime = 0.5f;
			m_fRotationCount = -50;
			m_fZMove = 70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 29:
			m_fPattonTime = 0.5f;
			m_fRotationCount = 200;
			m_fZMove = -70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 30:
			m_fPattonTime = 0.5f;
			m_fRotationCount = -200;
			m_fZMove = -70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 31:
			m_fZMove = 0.f;
			m_fPattonTime = 0.3f;
			m_ePattonState = MONSTER_HURT;
			m_tLastManInfo.eMainState = MONSTER_HURT;
			break;
		case 32:
			m_fPattonTime = 5.f;
			m_fRotationCount = 300;
			m_fZMove = 70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 33:
			m_fPattonTime = 5.f;
			m_fRotationCount = -300;
			m_fZMove = -70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 34:
			m_fZMove = 0.f;
			m_fPattonTime = 0.8f;
			m_ePattonState = MONSTER_SORK;
			m_tLastManInfo.eMainState = MONSTER_SORK;
			break;
		case 35:
			m_fPattonTime = 3.f;
			m_fRotationCount = 200;
			m_fZMove = 70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		case 36:
			m_fPattonTime = 3.f;
			m_fRotationCount = -200;
			m_fZMove = -70.f;
			m_ePattonState = MONSTER_MOVE;
			break;
		default:
			m_fPattonTime = 2.f;
			m_ePattonState = MONSTER_IDLE;
			break;
		}
	}
}

void CComputer_Goblin::Change_State(void)
{
	switch (m_tLastManInfo.eMainState)
	{
	case MONSTER_IDLE:
		m_iRenderState = GOBLIN_IDLE;
		break;
	case MONSTER_MOVE:
		m_iRenderState = GOBLIN_MOVE_FORWARD;
		break;
	case MONSTER_ATTACK:
		m_iRenderState = GOBLIN_ATTACK_1;
		break;
	case MONSTER_JUMP:
		m_iRenderState = GOBLIN_JUMP;
		break;
	case MONSTER_JUMP_FALLING:
		m_iRenderState = GOBLIN_JUMP;
		break;
	case MONSTER_DEAD:
		m_iRenderState = GOBLIN_DEATH;
		break;
	case MONSTER_SORK:
		m_iRenderState = GOBLIN_COUGH;
		break;
	case MONSTER_HURT:
		m_iRenderState = GOBLIN_HURT;
		break;
	}
}

void CComputer_Goblin::Control_PlayerMove(const float & fTimeDelta)
{
	if (0.f != m_tLastManInfo.tUnitInfo.fSpeedZ && 0.f == m_tLastManInfo.tUnitInfo.fSpeedX)
		m_dwIndex = m_pTransformCom->Go_Straight(fTimeDelta * m_tLastManInfo.tUnitInfo.fSpeedZ, m_dwIndex, false);
	if (0.f != m_tLastManInfo.tUnitInfo.fSpeedX && 0.f == m_tLastManInfo.tUnitInfo.fSpeedZ)
		m_dwIndex = m_pTransformCom->Go_Right(fTimeDelta * m_tLastManInfo.tUnitInfo.fSpeedX, m_dwIndex, false);
	if (0.f != m_tLastManInfo.tUnitInfo.fSpeedX && 0.f != m_tLastManInfo.tUnitInfo.fSpeedZ)
	{
		m_dwIndex = m_pTransformCom->Go_Straight(fTimeDelta * m_tLastManInfo.tUnitInfo.fSpeedZ * sqrtf(0.9f), m_dwIndex, false);
		m_dwIndex = m_pTransformCom->Go_Right(fTimeDelta * m_tLastManInfo.tUnitInfo.fSpeedX * sqrtf(0.9f), m_dwIndex, false);
	}

	if (3.f < m_tLastManInfo.tUnitInfo.fSpeedX)
		m_tLastManInfo.tUnitInfo.fSpeedX -= fTimeDelta * 200.f;
	else if (-3.f > m_tLastManInfo.tUnitInfo.fSpeedX)
		m_tLastManInfo.tUnitInfo.fSpeedX += fTimeDelta * 200.f;
	else
		m_tLastManInfo.tUnitInfo.fSpeedX = 0.f;

	if (3.f < m_tLastManInfo.tUnitInfo.fSpeedZ)
		m_tLastManInfo.tUnitInfo.fSpeedZ -= fTimeDelta * 200.f;
	else if (-3.f > m_tLastManInfo.tUnitInfo.fSpeedZ)
		m_tLastManInfo.tUnitInfo.fSpeedZ += fTimeDelta * 200.f;
	else
		m_tLastManInfo.tUnitInfo.fSpeedZ = 0.f;
}

void CComputer_Goblin::Check_Collision(void)
{
	if (!m_bFirstRender)
		return;

	if (m_tLastManInfo.eMainState == MONSTER_DEAD ||
		m_tLastManInfo.tUnitInfo.bDead)
		return;

	auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonWeapon");
	for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
	{
		if (m_pCollisionBody->Check_Collision((CCollisionSphere*)(*iter)))
		{
			((CCollisionSphere*)(*iter))->Set_Life((_float)m_tLastManInfo.iPlayerIdx);
			m_tLastManInfo.eMainState = MONSTER_DEAD;

			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_MonsterDeadGreenBlood", m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), m_pTransformCom->Get_WorldMatrix());

		}
	}
}

void CComputer_Goblin::Calculate_Radius(void)
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

void CComputer_Goblin::Check_Magnetic(void)
{
	if (LASTMAN_ROUND_START != g_eLastMan_Progress)
		return;

	_vec3 vecMagnetic = m_pDataMgr->Get_MagneticInitPos(g_iRound);

	_vec3 vecDir = m_pTransformCom->Get_Info(CTransform::INFO_POSITION) - vecMagnetic;

	float fRadius = D3DXVec3Length(&vecDir); // 자신과 자기장 중심과의 거리
}

HRESULT CComputer_Goblin::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*         pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_Goblin", Engine::CComponent::COM_STATIC, L"Com_Mesh");
	AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_AIBody", 10.f, m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), 1.f);
	m_pCollisionManager->Add_Component(L"Com_Col_AIBody", m_pCollisionBody);
	return NOERROR;
}

HRESULT CComputer_Goblin::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pDynamicMeshCom)
		return E_FAIL;

	_matrix         matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CComputer_Goblin* CComputer_Goblin::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, vector<_int> vecRamdom, _uint uIndex)
{
	CComputer_Goblin* pInstance = new CComputer_Goblin(pGraphicDev, uIndex);

	if (FAILED(pInstance->Ready_GameObject(vPosition, vecRamdom)))
	{
		MessageBox(0, L"CComputer_Goblin Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CComputer_Goblin::Free(void)
{
	Engine::Safe_Release(m_pNavMgr);
	Engine::Safe_Release(m_pFrustumMgr);
	Engine::Safe_Release(m_pDataMgr);
	Engine::Safe_Release(m_pCollisionManager);
	m_pCollisionManager->Delete_Component(L"Com_Col_AIBody", m_pCollisionBody);
	Engine::Safe_Release(m_pCollisionBody);

	m_pInfoSubject->UnSubscribe_Observer(m_pLastManObserver);
	Safe_Release(m_pLastManObserver);
	Engine::Safe_Release(m_pInfoSubject);

	CDynamicObject::Free();
}