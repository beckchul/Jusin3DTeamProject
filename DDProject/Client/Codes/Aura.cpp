#include "stdafx.h"
#include "Aura.h"
#include "CollisionManager.h"

#include "Effect.h"
#include "Effect_Mesh.h"
#include "Effect_Mesh_Shield.h"
#include "EffectMgr.h"

#include "UI_Skill.h"
#include "Object_Manager.h"

CAura::CAura(LPDIRECT3DDEVICE9 pGraphicDev)
: CStaticObject(pGraphicDev)
, m_pCollisionManager(CCollisionManager::GetInstance())
{
	m_pCollisionManager->Add_Ref();
}

CAura::~CAura(void)
{
}

HRESULT CAura::Ready_GameObject(_vec3 vPosition, TOWER_TYPE eType)
{
	m_eTowerType = eType;

	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPosition);
	Engine::CGameObject::Update_GameObject(0.f);

	switch (m_eTowerType)// 멜론 생성구간
	{
	case TOWER_TYPE_SLOWAURA: // 초록색
		m_list_EffectInsideAura = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_In_SlowAura", &_vec3(vPosition.x, vPosition.y + 5.f, vPosition.z));
		m_list_EffectAura			= Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_SlowAura", &m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
	
		m_pCollisionBody->Set_Damage(5);
		break;
	case TOWER_TYPE_LIGHTNINGAURA: // 파란색
		m_list_EffectInsideAura = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_In_LightingAura", &_vec3(vPosition.x, vPosition.y + 5.f, vPosition.z));
		m_list_EffectAura = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_LightingAura", &m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

		m_pCollisionBody->Set_Damage(7);
		break;
	case TOWER_TYPE_HEALINGAURA:	// 보라색
		m_list_EffectInsideAura = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_In_HealingAura", &_vec3(vPosition.x, vPosition.y + 5.f, vPosition.z));
		m_list_EffectAura = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_HealingAura", &m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
		
		m_pCollisionBody->Set_Damage(10);
		break;
	case TOWER_TYPE_DRAINAURA:			// 노란색
		m_list_EffectInsideAura = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_In_DrainAura", &_vec3(vPosition.x, vPosition.y + 5.f, vPosition.z));
		m_list_EffectAura = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_DrainAura", &m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

		m_pCollisionBody->Set_Damage(5);
		break;
	case TOWER_TYPE_ENRAGEAURA:
		m_list_EffectAura = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_EnrageAura", &m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

		break;
	}

	m_pUIObject = CUI_Skill::Create(m_pGraphicDev, m_eTowerType);
	m_pUIObject->Set_Pos(&_vec3(vPosition.x, vPosition.y + 39.f, vPosition.z));
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", m_pUIObject);

	m_fLifeTime = 30.f;
	return NOERROR;
}

int CAura::Update_GameObject(const float & fTimeDelta)
{
	if (m_fLifeTime <= 0.f)
	{
		// 멜론 소멸구간
		if (m_list_EffectAura.size() > 0)
			Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_list_EffectAura);
		if (m_list_EffectInsideAura.size() > 0)
			Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_list_EffectInsideAura);
		if (m_pUIObject != nullptr)
			m_pUIObject->Set_Dead();

		return 1;
	}

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);

	if (nullptr != m_pCollisionBody)
	{
		
		if (NULL != m_pCollisionBody->Get_Life())
		{
			m_fCount += fTimeDelta;
			if (m_fCount >= 1.f)
			{
				m_fCount = 0.f;
				--m_fLifeTime;
			}
			m_pCollisionBody->Set_Life((_float)0);
		}
	}

	return 0;
}

void CAura::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	if (nullptr != m_pCollisionBody)
		m_pCollisionBody->Set_MatrixWorldCollision(m_pTransformCom->Get_WorldMatrix());
}

HRESULT CAura::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	switch (m_eTowerType)
	{
	case TOWER_TYPE_SLOWAURA:
		AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_SlowAura", 40.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_SlowAura", m_pCollisionBody);
		break;
	case TOWER_TYPE_LIGHTNINGAURA:
		AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_LightningAura", 40.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_LightningAura", m_pCollisionBody);
		break;
	case TOWER_TYPE_HEALINGAURA:
		AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_HealingAura", 40.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_HealingAura", m_pCollisionBody);
		break;
	case TOWER_TYPE_DRAINAURA:
		AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_DrainAura", 40.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_DrainAura", m_pCollisionBody);
		break;
	case TOWER_TYPE_ENRAGEAURA:
		AddCollisionSphere(m_pCollisionBody, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_EnrageAura", 40.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
		m_pCollisionManager->Add_Component(L"Com_Col_EnrageAura", m_pCollisionBody);
		break;
	}
	return NOERROR;
}

HRESULT CAura::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CAura* CAura::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, TOWER_TYPE eType)
{
	CAura* pInstance = new CAura(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vPosition, eType)))
	{
		MessageBox(0, L"CAura Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CAura::Free(void)
{
	switch (m_eTowerType)
	{
	case TOWER_TYPE_SLOWAURA:
		m_pCollisionManager->Delete_Component(L"Com_Col_SlowAura", m_pCollisionBody);
		break;
	case TOWER_TYPE_LIGHTNINGAURA:
		m_pCollisionManager->Delete_Component(L"Com_Col_LightningAura", m_pCollisionBody);
		break;
	case TOWER_TYPE_HEALINGAURA:
		m_pCollisionManager->Delete_Component(L"Com_Col_HealingAura", m_pCollisionBody);
		break;
	case TOWER_TYPE_DRAINAURA:
		m_pCollisionManager->Delete_Component(L"Com_Col_DrainAura", m_pCollisionBody);
		break;
	case TOWER_TYPE_ENRAGEAURA:
		m_pCollisionManager->Delete_Component(L"Com_Col_EnrageAura", m_pCollisionBody);
		break;
	}
	Engine::Safe_Release(m_pCollisionBody);
	Engine::Safe_Release(m_pCollisionManager);
	cout << "오라 소멸" << endl;
	CStaticObject::Free();
}
