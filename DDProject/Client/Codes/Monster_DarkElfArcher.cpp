#include "stdafx.h"
#include "Monster_DarkElfArcher.h"
#include "Renderer.h"
#include "InfoSubject.h"
#include "PlayerObserver.h"
#include "CollisionManager.h"
#include "Monster_Arrow.h"
#include "Object_Manager.h"
#include "NavMgr.h"

#include "WaveObserver.h"
#include "DataManager.h"
#include "SoundMgr.h"

CMonster_DarkElfArcher::CMonster_DarkElfArcher(LPDIRECT3DDEVICE9 pGraphicDev)
: CMonster(pGraphicDev)
{
}

CMonster_DarkElfArcher::~CMonster_DarkElfArcher()
{
}

HRESULT CMonster_DarkElfArcher::Ready_GameObject(_vec3 & vPosition, Engine::OBJECTID eRoute)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;


	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPosition);

	m_tInfo = INFO(100, 100, 0, 0, 15, false, 12.f, 0.f, 0.f);
	m_tFllowTarget.fAttackRange = 90.f;
	m_tFllowTarget.fSearchLength = 150.f;

	m_pPlayerObserver = CPlayerObserver::Create();
	m_pInfoSubject->Subscribe_Observer(m_pPlayerObserver);
	m_pWaveObserver = CWaveObserver::Create(m_pGraphicDev);
	m_pInfoSubject->Subscribe_Observer(m_pWaveObserver);

	Load_AnimationInfo("../Resources/Data/AnimationData/DarkElfArcherData.xml");

	// NavMesh
	m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_eRouteID = eRoute;

	m_eMonsterType = MONSTER_DARKELF;

	m_vecTransColor = _vec4(1.f, -0.3f, -0.3f, 0.f);

	return NOERROR;
}

int CMonster_DarkElfArcher::FirstUpdate_GameObject(const float & fTimeDelta)
{
	return 0;
}

int CMonster_DarkElfArcher::Update_GameObject(const float & fTimeDelta)
{
	if (m_tInfo.bDead)
	{
		m_fDeadTime += fTimeDelta;

		m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(1.f - m_fDeadTime * 0.5f, 1.f - m_fDeadTime * 0.5f, 1.f - m_fDeadTime * 0.5f));

		if (m_fDeadTime >= 2.f)
		{
			m_pInfoSubject->Notify_Message(MESSAGE_MINUSCOUNT);
			Give_ManaToken();
			return 1;
		}
	}

	if (!m_bFirstFrame)
		CMonster::Create_Route(m_eRouteID);

	m_fTimeDelta = fTimeDelta;
	if (MONSTER_HURT != m_eMainState &&
		MONSTER_DEAD != m_eMainState)
		m_iRenderState = 0;

	if (MONSTER_DEAD == m_eMainState)
	{
		bool bDropKill = false;
		if (!m_pNavMgr->FallOnNavMesh(m_pTransformCom, m_dwIndex, bDropKill))
		{
			_vec3 vPosition = m_pTransformCom->Get_Info(CTransform::INFO_POSITION) + m_vDeadDir * fTimeDelta * 80.f;
			m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPosition);
		}
	}

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DYNAMICMESH_NONEALPHA, this);

	if (-1 == m_dwIndex)
		m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	if (CMonster::Search_Target(fTimeDelta))
		CMonster::Follow_Player(fTimeDelta);
	else
		CMonster::Work_Route(fTimeDelta);

	CMonster::Monster_Fall(fTimeDelta);
	Change_State();

	CMonster::Check_Collision(fTimeDelta);
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

	if (m_pWaveObserver->Get_WaveInfo().bWaveClear)
		m_eMainState = MONSTER_DEAD;

	return 0;
}

void CMonster_DarkElfArcher::FirstRender_GameObject(_bool bTrue)
{
	if (!bTrue)
		CMonster::Play_Animation(m_fTimeDelta); 
	m_pCollisionBody->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());
}

void CMonster_DarkElfArcher::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{	
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	if (m_tInfo.bDead)
	{
		m_pDynamicMeshCom->Set_TrackPosition(m_iDeadTrackIndex, m_fDeadTrackPosition);
		m_pDynamicMeshCom->Play_AnimationSet(0.f);
		m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);
	}
	else
		m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);

	m_bFirstFrame = true;
}
HRESULT CMonster_DarkElfArcher::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;
	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_DarkElfArcher", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.CollisionSphere
	AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_MonBody", 13.f, m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), 1.5f);
	m_pCollisionManager->Add_Component(L"Com_Col_MonBody", m_pCollisionBody);
	
	return NOERROR;
}

HRESULT CMonster_DarkElfArcher::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

void CMonster_DarkElfArcher::Change_State(void)
{
	if (m_eMainState == MONSTER_DEAD)
		m_iRenderState = DARKELFARCHER_DEATH;
	else if (m_eMainState == MONSTER_HURT)
		m_iRenderState = DARKELFARCHER_HURT;
	else if (m_eMainState == MONSTER_TURN || m_eMainState == MONSTER_MOVE)
		m_iRenderState = DARKELFARCHER_MOVE_FORWARD;
	else if (m_eMainState == MONSTER_ATTACK || m_eMainState == MONSTER_ATTACK_2)
		m_iRenderState = DARKELFARCHER_ATTACK;
	else
		m_iRenderState = DARKELFARCHER_IDLE;
}

void CMonster_DarkElfArcher::Set_Collision(void)
{
	if (m_iRenderState == DARKELFARCHER_ATTACK && m_pDynamicMeshCom->GetTrackPosition() >= 1.6f)
	{
		if (0 == m_iAttackState)
		{
			// 화살 생성
			++m_iAttackState;

			Engine::CGameObject*		pGameObject = nullptr;
			_vec3 vPosition = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
			vPosition.y += 14.f;
			_vec3 vAngle = m_pTransformCom->Get_Info(CTransform::INFO_ANGLE);
			vAngle.y += D3DXToRadian(180.f);
			pGameObject = CMonster_Arrow::Create(m_pGraphicDev, vPosition, vAngle);
			((CMonster_Arrow*)pGameObject)->Set_Damage((int)(m_tInfo.iAttPower * m_fLowDamage));
			if (nullptr == pGameObject)
				return;
			if (FAILED(Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
				return;

			_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
			float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
			if (fDistance < 250.f)
			{
				if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MONSTER_ATTACK2))
					CSoundMgr::GetInstance()->Play_RandomSound(L"DarkElf_Arrow.ogg", Engine::CHANNEL_MONSTER_ATTACK2, 3, 0.6f);
			}
		}
	}

}


CMonster_DarkElfArcher* CMonster_DarkElfArcher::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 & vPosition, Engine::OBJECTID eRoute)
{
	CMonster_DarkElfArcher* pInstance = new CMonster_DarkElfArcher(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPosition, eRoute)))
	{
		MessageBox(0, L"CMonster_DarkElfArcher Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CMonster_DarkElfArcher::Free(void)
{
	m_pInfoSubject->UnSubscribe_Observer(m_pPlayerObserver);
	Safe_Release(m_pPlayerObserver);
	
	m_pInfoSubject->UnSubscribe_Observer(m_pWaveObserver);
	Safe_Release(m_pWaveObserver);


	m_pCollisionManager->Delete_Component(L"Com_Col_MonBody", m_pCollisionBody);
	Engine::Safe_Release(m_pCollisionBody);
	CMonster::Free();
}
