#include "stdafx.h"
#include "TresureBox.h"
#include "AnimationCtrl.h"
#include "PlayerObserver.h"
#include "WaveObserver.h"
#include "InfoSubject.h"
#include "Present.h"
#include "MyEffect.h"
#include "Object_Manager.h"
#include "SoundMgr.h"

USING(Engine)

CTresureBox::CTresureBox(LPDIRECT3DDEVICE9 pGraphicDev)
: CDynamicObject(pGraphicDev)
, m_bOpen(false)
, m_ePhaseType(PHASE_END)
, m_pPlayerObserver(nullptr)
, m_pWaveObserver(nullptr)
, m_fPresentTime(-1.f)
{
}

CTresureBox::~CTresureBox(void)
{
}

HRESULT CTresureBox::Ready_GameObject()
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.1f, 0.1f, 0.1f));

	ANI_INFO tAniInfo = ANI_INFO(0, 0.5f, 0.5f, 0.9f, 0.1f, 0.05, 0.05);
	m_vecAnimationInfo.push_back(tAniInfo);
	m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[0]);

	m_pPlayerObserver = CPlayerObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	m_pWaveObserver = CWaveObserver::Create(m_pGraphicDev);
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pWaveObserver);

	return NOERROR;
}

int CTresureBox::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);
	m_fTimeDelta = fTimeDelta;

	Give_Present(fTimeDelta); // 선물 뿌리기

	if (FALSE == m_bOpen) // 닫혀있는 상태
	{
		// 플레이어 근접하면 열린다
		for (int i = 0; i < MAX_CLIENT_NUM; ++i)
		{
			const PLAYER_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(i);
			if (nullptr == pInfo)
				continue;

			_vec3 vecDir = pInfo->pTransform->Get_Info(CTransform::INFO_POSITION) - m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
			float fDistance = D3DXVec3Length(&vecDir);

			if (fDistance < 20.f)
			{
				m_bOpen = TRUE;
				m_fPresentTime = 0.5f; // 선물을 0.5초뒤에 줌
			}
		}
	}
	else // 열려있는 상태
	{
		// 웨이브가 끝나면 닫힌다

		PHASE_TYPE ePhase = m_pWaveObserver->Get_WaveInfo().ePhaseType;

		if (BUILD_PHASE == ePhase)
		{
			if (COMBAT_PHASE == m_ePhaseType) // 전투 -> Build가 된 경우
				m_bOpen = FALSE;
		}

		m_ePhaseType = ePhase;
	}
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DYNAMICMESH_NONEALPHA, this);

	return 0;
}

void CTresureBox::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	Play_Animation(m_fTimeDelta);

	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

void CTresureBox::Play_Animation(const float& fTimeDelta)
{
	if (m_bOpen)
	{
		if(m_pDynamicMeshCom->GetPeriod() - m_vecAnimationInfo[0].dEndAniCount >= m_pDynamicMeshCom->GetTrackPosition())
			m_pDynamicMeshCom->Play_AnimationSet(fTimeDelta);
		else
			m_pDynamicMeshCom->Play_AnimationSet(0.f);
	}
	else
	{
		m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[0]);
		m_pDynamicMeshCom->Play_AnimationSet(0.1f);
	}
}

void CTresureBox::Give_Present(const float & fTimeDelta)
{
	if (-1.f == m_fPresentTime)
		return;

	m_fPresentTime -= fTimeDelta;

	if (m_fPresentTime >= 0.f)
		return;

	CSoundMgr::GetInstance()->Play_Sound(L"TresureBox.ogg", Engine::CHANNEL_TRESUREBOX, 0.4f);
	m_fPresentTime = -1.f;

	// _vec3(12.f, 15.f, 23.f)
	for (int i = 0; i < 4; ++i)
	{
		Engine::CGameObject* pPresent = CPresent::Create(m_pGraphicDev);
		if (nullptr == pPresent)
			return;

		dynamic_cast<CPresent*>(pPresent)->Set_Dir(_vec3(-20.f + rand() % 40, 5 + float(rand() % 15), -20.f + rand() % 40));
		pPresent->Set_Pos(&(m_pTransformCom->Get_Info(CTransform::INFO_POSITION) + _vec3(0.f, 20.f, 0.f)));

		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pPresent);
	}

	// 임시 이펙트
	Engine::CGameObject* pEffect = CMyEffect::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return;

	pEffect->Set_Pos(&(m_pTransformCom->Get_Info(CTransform::INFO_POSITION) + _vec3(0.f, 10.f, 0.f)));

	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pEffect);
}

HRESULT CTresureBox::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_TresureChest", Engine::CComponent::COM_STATIC, L"Com_Mesh");
	return NOERROR;
}

HRESULT CTresureBox::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CTresureBox * CTresureBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTresureBox* pInstance = new CTresureBox(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CTresureBox Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CTresureBox::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pWaveObserver);
	Engine::Safe_Release(m_pWaveObserver);
	
	CDynamicObject::Free();
}
