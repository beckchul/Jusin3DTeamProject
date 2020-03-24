#include "stdafx.h"
#include "Blockade.h"
#include "CollisionManager.h"
#include "EffectMgr.h"

CBlockade::CBlockade(LPDIRECT3DDEVICE9 pGraphicDev)
: CTower(pGraphicDev) 
{
}

CBlockade::~CBlockade()
{
}

HRESULT CBlockade::Ready_GameObject()
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	m_tInfo = INFO(600, 600, 0, 0, 0, false, 0.f, 0.f, 0.f);

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.f, 0.f, 0.f));

	m_eMainState = TOWER_IDLE;
	m_pCollisionBody->Set_CheckCollsion(true);

	Load_AnimationInfo("../Resources/Data/AnimationData/BlockadeData.xml");

	m_vecTransColor = _vec4(1.f, -0.3f, -0.3f, 0.f);

	return NOERROR;
}

int CBlockade::Update_GameObject(const float & fTimeDelta)
{
	if (m_tInfo.bDead || m_bDead)
	{
		//파괴 이펙트

		if (m_vecPrevColor == _vec4(1.f, 1.f, 1.f, 1.f))
		{
			_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Tower_Destruction", &vPos);
		}
		return 1;
	}

	Appear(m_fTimeDelta, 0.11f);

	m_fTimeDelta = fTimeDelta;
	if (TOWER_FIRE != m_eMainState)
		m_iRenderState = 0;

	Engine::CGameObject::Update_GameObject(fTimeDelta);
	CTower::Check_Collision();


	// 충돌 체크 후 색깔 지정.
	if (m_bColorAdd && !m_bSummoning)
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

	if (nullptr != m_pRendererCom)
	{
		if (TRUE == m_bSummoning) // 타워 소환중
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_ALPHA, this);
		else
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DYNAMICMESH_NONEALPHA, this);
	}
	return 0;
}

void CBlockade::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	CTower::Play_Animation(m_fTimeDelta);
	m_pCollisionBody->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());

	if (TRUE == m_bSummoning) // 타워 소환중
	{
		LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
		if (nullptr == pEffect)
			return;
		pEffect->AddRef();
		

		// 2초동안 색깔 원복
		m_vecPrevColor += _vec4(1.f, 0.f, 0.f, 1.f) * m_fTimeDelta * 0.5f; 

		if (FAILED(Set_ConstantTable(pEffect)))
			return;

		// 여기서 카메라 위치 전달. 포워드로 해야함.
		pEffect->SetVector("g_worldCameraPosition", (_vec4*)&CDataManager::GetInstance()->Get_CamPoistion());
		
		pEffect->Begin(nullptr, 0);

		m_pDynamicMeshCom->Render_Mesh(pEffect, 3);

		pEffect->End();
		

		Safe_Release(pEffect);
	}
	else
	{
		if (nullptr == pArgEffect)
			return;

		if (FAILED(Set_ConstantTable(pArgEffect)))
			return;

		pArgEffect->SetVector("g_fHitColor", &m_vecColor);

		m_pDynamicMeshCom->Render_Mesh(pArgEffect, uPassIdx);


		pArgEffect->SetVector("g_fColor", &_vec4(1.f, 1.f, 1.f, 1.f)); // 색상 원복(다른 Dynamic Mesh들)
	}
}

HRESULT CBlockade::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*         pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_Blockade", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_DynamicMesh", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.CollisionSphere
	AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Tower", 180.f, m_pDynamicMeshCom->Get_FrameMatrix("Root"), 0.15f);
	m_pCollisionManager->Add_Component(L"Com_Col_Tower", m_pCollisionBody);
	return NOERROR;
}

HRESULT CBlockade::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pDynamicMeshCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	pEffect->SetVector("g_fColor", &m_vecPrevColor);

	return NOERROR;
}

CBlockade* CBlockade::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBlockade* pInstance = new CBlockade(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CBlockade Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CBlockade::Free(void)
{
	m_pCollisionManager->Delete_Component(L"Com_Col_Tower", m_pCollisionBody);
	CTower::Free();
}
