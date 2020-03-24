#include "stdafx.h"
#include "BlockGroup.h"
#include "Renderer.h"
#include "DataManager.h"
#include "FrustumManager.h"
#include "CollisionManager.h"

USING(Engine)

CBlockGroup::CBlockGroup(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
	: CStaticObject(pGraphicDev)
	, m_iTextureNum(iIndex)
	, m_pFrustumMgr(CFrustumManager::GetInstance())
	, m_bLocalCenter(true)
{
	m_pFrustumMgr->Add_Ref();
}

CBlockGroup::~CBlockGroup()
{
}

HRESULT CBlockGroup::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return NOERROR;
}

int CBlockGroup::FirstUpdate_GameObject(const float & fTimeDelta)
{
	Calculate_Raidus(); // 반지름 구하기

	if (m_pTransformCom->Get_Info(CTransform::INFO_POSITION) == _vec3(0.f, 0.f, 0.f))
		m_bLocalCenter = false;

	Calculate_LocalPos(); // 로컬 좌표 구하기

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	CCollisionManager::GetInstance()->Add_MouseCol(m_pGraphicDev, m_pStaticMeshCom->Get_Mesh(), *m_pTransformCom->Get_WorldMatrix());

	return 0;
}

int CBlockGroup::Update_GameObject(const float & fTimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		if (FALSE == m_bLocalCenter) // 로컬 좌표만 있는 애들
		{
			if (CFrustumManager::GetInstance()->IsSphereInFrustum(m_vecLocalPos, m_fRadius))
				m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);
		}
			
		else // 월드 좌표가 있는 애들
		{
			if (CFrustumManager::GetInstance()->IsSphereInFrustum(m_pTransformCom->Get_Info(CTransform::INFO_POSITION), m_fRadius))
				m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);
		}
	}

	return 0;
}

void CBlockGroup::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

void CBlockGroup::Calculate_Raidus(void)
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

void CBlockGroup::Calculate_LocalPos(void)
{
	if (TRUE == m_bLocalCenter)
		return;

	// 위치를 구해보자
	LPD3DXMESH	pMesh = nullptr;

	m_pStaticMeshCom->Get_Mesh()->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_VTXPOS, m_pGraphicDev, &pMesh);

	Engine::VTXPOS* pVertex = nullptr;

	pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertex); // Lock

	_vec3 vecTemp, vecScale;

	vecScale = m_pTransformCom->Get_Info(CTransform::INFO_SCALE);

	size_t iSize = pMesh->GetNumVertices();
	for (size_t i = 0; i < iSize; ++i) // 모든 메쉬 정점의 평균으로 그 물체의 위치를 정한다
		vecTemp += pVertex[i].vPos;
	vecTemp /= (_float)iSize;

	m_vecLocalPos = _vec3(vecTemp.x * vecScale.x, vecTemp.y * vecScale.y, vecTemp.z * vecScale.z);

	pMesh->UnlockVertexBuffer(); // UnLock

	Engine::Safe_Release(pMesh);

	m_vecLocalPos;
}

HRESULT CBlockGroup::Add_Component(void)
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

HRESULT CBlockGroup::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CBlockGroup * CBlockGroup::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
{
	CBlockGroup* pInstance = new CBlockGroup(pGraphicDev, iIndex);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CBlockGroup Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CBlockGroup::Free(void)
{
	Engine::Safe_Release(m_pFrustumMgr);

	CStaticObject::Free();
}
