#include "stdafx.h"
#include "RightDoor.h"
#include "AnimationCtrl.h"
#include "PlayerObserver.h"

#include "InfoSubject.h"
#include "WaveObserver.h"

USING(Engine)

CRightDoor::CRightDoor(LPDIRECT3DDEVICE9 pGraphicDev)
: CDynamicObject(pGraphicDev)
, m_eState(DOOR_IDLE)
, m_eMainState(DOOR_IDLE)
, m_pWaveObserver(nullptr)
, m_eCurPhaseType(PHASE_END)
{
}

CRightDoor::~CRightDoor(void)
{
}

HRESULT CRightDoor::Ready_GameObject()
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.1f, 0.1f, 0.1f));
	for (_uint i = 0; i < 5; ++i)
	{
		ANI_INFO tAniInfo = ANI_INFO(i, 1.f, 1.f, 0.9f, 0.1f, 0.05, 0.1);
		m_vecAnimationInfo.push_back(tAniInfo);
	}
	m_eState = DOOR_CLOSED;
	m_eMainState = DOOR_CLOSED;
	m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[m_eState]);


	m_pWaveObserver = CWaveObserver::Create(m_pGraphicDev);
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pWaveObserver);

	return NOERROR;
}

int CRightDoor::FirstUpdate_GameObject(const float & fTimeDelta)
{
	_vec3 vDir = m_pTransformCom->Get_Info(CTransform::INFO_ANGLE);
	_float x = D3DXToDegree(vDir.x);
	_float y = D3DXToDegree(vDir.y);
	_float z = D3DXToDegree(vDir.z);
	if(90 == x || -90 == x)
		m_pTransformCom->Change_Information(CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), D3DXToRadian(180.f), D3DXToRadian(0.f)));
	else
		m_pTransformCom->Change_Information(CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(-90.f), D3DXToRadian(180.f), D3DXToRadian(0.f)));
	return 0;
}

int CRightDoor::Update_GameObject(const float & fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
	if (m_eCurPhaseType != m_pWaveObserver->Get_WaveInfo().ePhaseType)
	{
		m_eCurPhaseType = m_pWaveObserver->Get_WaveInfo().ePhaseType;
		if (m_eCurPhaseType == COMBAT_PHASE)
		{
			m_eMainState = DOOR_OPENING;
			m_eState = DOOR_OPENING;
			m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[m_eState]);
		}
		else if (m_eCurPhaseType == BUILD_PHASE || m_eCurPhaseType == WAVE_COMPLETE)
		{
			m_eMainState = DOOR_CLOSING;
			m_eState = DOOR_CLOSING;
			m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[m_eState]);
		}
	}
	

	Engine::CGameObject::Update_GameObject(fTimeDelta);
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DYNAMICMESH_NONEALPHA, this);

	return 0;
}

void CRightDoor::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	Play_Animation(m_fTimeDelta);

	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

void CRightDoor::Play_Animation(const float& fTimeDelta)
{
	if (m_pDynamicMeshCom->GetPeriod() - m_vecAnimationInfo[m_eState].dEndAniCount <= m_pDynamicMeshCom->GetTrackPosition())
	{
		switch (m_eMainState)
		{
		case DOOR_OPENING:
		case DOOR_OPEN:
			m_eState = DOOR_OPEN;
			break;
		case DOOR_CLOSING:
		case DOOR_CLOSED:
			m_eState = DOOR_CLOSED;
			break;
		}
		m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[m_eState]);
	}

	m_pDynamicMeshCom->Play_AnimationSet(fTimeDelta);
}

HRESULT CRightDoor::Add_Component(void)
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
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STAGE, L"Com_Mesh_RightDoor", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CRightDoor::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CRightDoor * CRightDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRightDoor* pInstance = new CRightDoor(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CRightDoor Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CRightDoor::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pWaveObserver);
	Engine::Safe_Release(m_pWaveObserver);

	CDynamicObject::Free();
}
