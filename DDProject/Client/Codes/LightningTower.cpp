#include "stdafx.h"
#include "LightningTower.h"
#include "CollisionManager.h"
#include "Object_Manager.h"
#include "EffectMgr.h"
#include "Effect.h"
#include "SplashDamage.h"

CLightningTower::CLightningTower(LPDIRECT3DDEVICE9 pGraphicDev)
: CTower(pGraphicDev) 
{
}

CLightningTower::~CLightningTower()
{
}

HRESULT CLightningTower::Ready_GameObject()
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	m_tInfo = INFO(200, 200, 0, 0, 20, false, 0.f, 0.f, 0.f);

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.f, 0.f, 0.f));

	m_tTargetCheck.fRength = 170.f;
	m_fAttackDelayCount = 10.f;
	m_tTargetCheck.fAngle = 360.f;
	m_pCollisionBody->Set_CheckCollsion(true);

	Load_AnimationInfo("../Resources/Data/AnimationData/FireTowerData.xml");

	m_vecTransColor = _vec4(1.f, -0.3f, -0.3f, 0.f);

	return NOERROR;
}

int CLightningTower::Update_GameObject(const float & fTimeDelta)
{
	if (m_tInfo.bDead || m_bDead)
	{
		//�ı� ����Ʈ
		// UnderEffect & BallEffect �����ش�.
		if (m_vecPrevColor == _vec4(1.f, 1.f, 1.f, 1.f))
		{
			_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Tower_Destruction", &vPos);
		}	
		if (m_pTower_BallEffect.size() > 0)
		{
			Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_pTower_BallEffect);
		}
		if (m_pTower_UnderEffect.size() > 0)
		{
			Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_pTower_UnderEffect);
		}
		return 1;
	}
	Appear(m_fTimeDelta, 0.14f);

	if (m_tTargetCheck.bTarget)
		m_eMainState = TOWER_FIRE;
	else
		m_eMainState = TOWER_IDLE;

	Change_State();
	Create_Bullet();

	m_fTimeDelta = fTimeDelta;
	if (TOWER_FIRE != m_eMainState)
		m_iRenderState = 0;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if (TRUE == m_bSummoning) // Ÿ�� ��ȯ��
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_ALPHA, this);
		else
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DYNAMICMESH_NONEALPHA, this);
	}
	CTower::Check_Target();
	CTower::Check_Collision();

	// �浹 üũ �� ���� ����.
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

	return 0;
}

void CLightningTower::FirstRender_GameObject(_bool bTrue)
{
	if (!bTrue)
		CTower::Play_Animation(m_fTimeDelta);
	m_pCollisionBody->Update_CollisionSphere(m_pTransformCom->Get_WorldMatrix());
}

void CLightningTower::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{

	if (TRUE == m_bSummoning) // Ÿ�� ��ȯ��
	{
		LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
		if (nullptr == pEffect)
			return;
		pEffect->AddRef();


		// 2�ʵ��� ���� ����
		m_vecPrevColor += _vec4(1.f, 0.f, 0.f, 1.f) * m_fTimeDelta * 0.5f;

		if (FAILED(Set_ConstantTable(pEffect)))
			return;

		// ���⼭ ī�޶� ��ġ ����. ������� �ؾ���.
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

		pArgEffect->SetVector("g_fColor", &_vec4(1.f, 1.f, 1.f, 1.f)); // ���� ����(�ٸ� Dynamic Mesh��)
	}
}

HRESULT CLightningTower::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*         pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DynamicMesh
	AddComponent(m_pDynamicMeshCom, Engine::CDynamicMesh*, SCENE_STATIC, L"Com_Mesh_LightningTower", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_DynamicMesh", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.CollisionSphere
	AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Tower", 120.f, m_pDynamicMeshCom->Get_FrameMatrix("Root"), 0.15f);
	m_pCollisionManager->Add_Component(L"Com_Col_Tower", m_pCollisionBody);
	return NOERROR;
}

HRESULT CLightningTower::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

void CLightningTower::Change_State(void)
{
	if (TOWER_FIRE == m_eMainState)
		m_iRenderState = TOWER_FIRE;
	else
		m_iRenderState = TOWER_IDLE;
}

void CLightningTower::Create_Bullet(void)
{
	if (m_iRenderState == TOWER_FIRE)
	{
		if (NULL == m_iAttackCount)
		{
			++m_iAttackCount;

			_matrix* matHand = m_pDynamicMeshCom->Get_FrameMatrix("Root");
			(*matHand) *= (*m_pTransformCom->Get_WorldMatrix());

			_vec3 vHandPos(0.f, 0.f, 0.f);
			memcpy(&vHandPos, &matHand->m[3][0], sizeof(_vec3));

			Engine::CGameObject*		pGameObject = nullptr;
			pGameObject = CSplashDamage::Create(m_pGraphicDev, vHandPos, MAGE_LIGHTNING_TOWER_ATTACK);
			((CSplashDamage*)pGameObject)->Set_Damage(m_tInfo.iAttPower);
			Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject);
		}
	}
}

CLightningTower* CLightningTower::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLightningTower* pInstance = new CLightningTower(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CLightningTower Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CLightningTower::Free(void)
{
	m_pCollisionManager->Delete_Component(L"Com_Col_Tower", m_pCollisionBody);
	CTower::Free();
}