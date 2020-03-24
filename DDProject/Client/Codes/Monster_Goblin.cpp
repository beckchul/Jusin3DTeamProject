#include "stdafx.h"
#include "Monster_Goblin.h"
#include "Renderer.h"
#include "InfoSubject.h"
#include "PlayerObserver.h"
#include "CollisionManager.h"
#include "NavMgr.h"

#include "WaveObserver.h"
#include "SoundMgr.h"
#include "DataManager.h"
USING(Engine)

CMonster_Goblin::CMonster_Goblin(LPDIRECT3DDEVICE9 pGraphicDev)
: CMonster(pGraphicDev)
{

}


CMonster_Goblin::~CMonster_Goblin()
{
}

HRESULT CMonster_Goblin::Ready_GameObject(_vec3& vPosition, Engine::OBJECTID eRoute)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPosition, true);
	m_tInfo = INFO(80, 80, 0, 0, 10, false, 15.f, 0.f, 0.f);

	m_pPlayerObserver = CPlayerObserver::Create();
	m_pInfoSubject->Subscribe_Observer(m_pPlayerObserver);
	m_pWaveObserver = CWaveObserver::Create(m_pGraphicDev);
	m_pInfoSubject->Subscribe_Observer(m_pWaveObserver);

	Load_AnimationInfo("../Resources/Data/AnimationData/GoblinData.xml");

	m_eRouteID = eRoute;

	// NavMesh
	m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_eMonsterType = MONSTER_GOBLIN;

	m_vecTransColor = _vec4(1.f, -0.3f, -0.3f, 0.f);

	return NOERROR;
}

int CMonster_Goblin::FirstUpdate_GameObject(const float & fTimeDelta)
{
	return 0;
}

int CMonster_Goblin::Update_GameObject(const float& fTimeDelta)
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
	if(MONSTER_HURT != m_eMainState &&
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

	CMonster::Monster_Fall(fTimeDelta);

	if (CMonster::Search_Target(fTimeDelta))
		CMonster::Follow_Player(fTimeDelta);
	else
		CMonster::Work_Route(fTimeDelta);

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

void CMonster_Goblin::FirstRender_GameObject(_bool bTrue)
{
	if (!bTrue)
		CMonster::Play_Animation(m_fTimeDelta);
	m_pCollisionBody->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());
	m_pCollsionWeapon->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());
	m_pCollsionWeapon->Set_Damage((int)(m_tInfo.iAttPower * m_fLowDamage));
}

void CMonster_Goblin::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
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

HRESULT CMonster_Goblin::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	//// For.Shader
	//AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_DynamicMesh", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_Goblin", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.CollisionSphere
	AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_MonBody", 10.f, m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), 1.f);
	AddCollisionSphere(m_pCollsionWeapon, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_MonWeapon", 8.f, m_pDynamicMeshCom->Get_FrameMatrix("b_FingersR"), 1.f);
	m_pCollisionManager->Add_Component(L"Com_Col_MonBody", m_pCollisionBody);
	m_pCollisionManager->Add_Component(L"Com_Col_MonWeapon", m_pCollsionWeapon);
	m_pCollsionWeapon->Set_CheckCollsion(false);
	return NOERROR;
}

HRESULT CMonster_Goblin::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

void CMonster_Goblin::Change_State()
{
	if (m_eMainState == MONSTER_DEAD)
		m_iRenderState = GOBLIN_DEATH;
	else if (m_eMainState == MONSTER_HURT)
		m_iRenderState = GOBLIN_HURT;
	else if (m_eMainState == MONSTER_TURN)
		m_iRenderState = GOBLIN_MOVE_FORWARD;
	else if (m_eMainState == MONSTER_ATTACK)
		m_iRenderState = GOBLIN_ATTACK;
	else if (m_eMainState == MONSTER_ATTACK_2)
		m_iRenderState = GOBLIN_ATTACK_1;
	else if (m_eMainState == MONSTER_MOVE)
		m_iRenderState = GOBLIN_MOVE_FORWARD;
	else
		m_iRenderState = GOBLIN_IDLE;
}

void CMonster_Goblin::Set_Collision(void)
{
	
	if (m_iRenderState == GOBLIN_ATTACK && m_pDynamicMeshCom->GetTrackPosition() >= 0.7f && m_pDynamicMeshCom->GetTrackPosition() <= 0.9f)
	{
		m_pCollsionWeapon->Set_CheckCollsion(true);
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 250.f)
		{
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MONSTER_ATTACK))
				CSoundMgr::GetInstance()->Play_RandomSound(L"Goblin_attack.ogg", Engine::CHANNEL_MONSTER_ATTACK, 5, 0.5f);
		}
	}
	else if (m_iRenderState == GOBLIN_ATTACK_1 && m_pDynamicMeshCom->GetTrackPosition() >= 0.6f && m_pDynamicMeshCom->GetTrackPosition() <= 0.8f)
	{
		m_pCollsionWeapon->Set_CheckCollsion(true);
		_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
		float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
		if (fDistance < 250.f)
		{
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_MONSTER_ATTACK))
				CSoundMgr::GetInstance()->Play_RandomSound(L"Goblin_attack.ogg", Engine::CHANNEL_MONSTER_ATTACK, 5, 0.5f);
		}
	}
	else
		m_pCollsionWeapon->Set_CheckCollsion(false);
}

CMonster_Goblin* CMonster_Goblin::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3& vPosition, Engine::OBJECTID eRoute)
{
	CMonster_Goblin* pInstance = new CMonster_Goblin(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPosition, eRoute)))
	{
		MessageBox(0, L"CMonster_Goblin Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CMonster_Goblin::Free(void)
{
	m_pInfoSubject->UnSubscribe_Observer(m_pPlayerObserver);
	Safe_Release(m_pPlayerObserver);

	m_pInfoSubject->UnSubscribe_Observer(m_pWaveObserver);
	Safe_Release(m_pWaveObserver);


	m_pCollisionManager->Delete_Component(L"Com_Col_MonBody", m_pCollisionBody);
	m_pCollisionManager->Delete_Component(L"Com_Col_MonWeapon", m_pCollsionWeapon);
	Engine::Safe_Release(m_pCollisionBody);
	Engine::Safe_Release(m_pCollsionWeapon);
	CMonster::Free();
}
