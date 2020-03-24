#include "stdafx.h"
#include "3DMaxObject.h"
#include "Renderer.h"
#include "DataManager.h"
#include "FrustumManager.h"
#include "InputDev.h"
#include "CollisionManager.h"

USING(Engine)

bool C3DMaxObject::m_bShowMap = false;

C3DMaxObject::C3DMaxObject(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
	: CStaticObject(pGraphicDev)
	, m_iTextureNum(iIndex)
	, m_pFrustumMgr(CFrustumManager::GetInstance())
{
	m_pFrustumMgr->Add_Ref();
}


C3DMaxObject::~C3DMaxObject()
{
}

HRESULT C3DMaxObject::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return NOERROR;
}

int C3DMaxObject::FirstUpdate_GameObject(const float & fTimeDelta)
{
	_vec3 vecMin, vecMax, vecScale;
	m_pStaticMeshCom->Compute_MinMax(&vecMin, &vecMax);

	vecScale = m_pTransformCom->Get_Info(CTransform::INFO_SCALE);

	vecMin.x *= vecScale.x;
	vecMin.y *= vecScale.y;
	vecMin.z *= vecScale.z;

	vecMax.x *= vecScale.x;
	vecMax.y *= vecScale.y;
	vecMax.z *= vecScale.z;

	m_fRadius = D3DXVec3Length(&(vecMax - vecMin)) / 2.f;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	CCollisionManager::GetInstance()->Add_MouseCol(m_pGraphicDev, m_pStaticMeshCom->Get_Mesh(), *m_pTransformCom->Get_WorldMatrix());

	return 0;
}

int C3DMaxObject::Update_GameObject(const float & fTimeDelta)
{
#ifndef Demonstration
	if(Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_NUMPAD2))
		m_bShowMap = (m_bShowMap == TRUE) ? FALSE : TRUE;

	if(FALSE == m_bShowMap)
		return 0;
#endif

	if (nullptr != m_pRendererCom)
	{
		if (m_pFrustumMgr->IsSphereInFrustum(m_pTransformCom->Get_Info(CTransform::INFO_POSITION), m_fRadius))
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);
	}
	return 0;
}

void C3DMaxObject::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

HRESULT C3DMaxObject::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STAGE, Engine::CDataManager::GetInstance()->Get_ObjName()[m_iTextureNum - Engine::OBJECT_END]->szObjectName, Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT C3DMaxObject::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

C3DMaxObject * C3DMaxObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
{
	C3DMaxObject* pInstance = new C3DMaxObject(pGraphicDev, iIndex);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"C3DMaxObject Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void C3DMaxObject::Free(void)
{
	Engine::Safe_Release(m_pFrustumMgr);

	CStaticObject::Free();
}
