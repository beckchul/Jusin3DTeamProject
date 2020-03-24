#include "stdafx.h"
#include "Ribbon.h"
#include "Renderer.h"
#include "DataManager.h"
#include "FrustumManager.h"
#include "CollisionManager.h"

USING(Engine)

CRibbon::CRibbon(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
	: CStaticObject(pGraphicDev)
	, m_iTextureNum(iIndex)
	, m_pFrustumMgr(CFrustumManager::GetInstance())
	, m_fTime(0.f)
{
	m_pFrustumMgr->Add_Ref();
}

CRibbon::~CRibbon()
{
}

HRESULT CRibbon::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fTime = RandFloat;

	return NOERROR;
}

int CRibbon::FirstUpdate_GameObject(const float & fTimeDelta)
{
	Calculate_Raidus();

	CCollisionManager::GetInstance()->Add_MouseCol(m_pGraphicDev, m_pStaticMeshCom->Get_Mesh(), *m_pTransformCom->Get_WorldMatrix());

	return 0;
}

int CRibbon::Update_GameObject(const float & fTimeDelta)
{
	m_fTime += fTimeDelta / 5.f;
	_float fMove = 10 * cosf(D3DXToRadian(m_fTime * 180.f)) * fTimeDelta;
	m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &_vec3(fMove, fMove, fMove));

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if (CFrustumManager::GetInstance()->IsSphereInFrustum(m_pTransformCom->Get_Info(CTransform::INFO_POSITION), m_fRadius))
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);
	}

	return 0;
}

void CRibbon::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

void CRibbon::Calculate_Raidus(void)
{
	// 반지름 구하기
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
}

HRESULT CRibbon::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STAGE2, Engine::CDataManager::GetInstance()->Get_ObjName2()[m_iTextureNum - Engine::OBJECT2_END]->szObjectName, Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CRibbon::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CRibbon * CRibbon::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
{
	CRibbon* pInstance = new CRibbon(pGraphicDev, iIndex);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CRibbon Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CRibbon::Free(void)
{
	Engine::Safe_Release(m_pFrustumMgr);

	CStaticObject::Free();
}
