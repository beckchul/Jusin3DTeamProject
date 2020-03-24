#include "stdafx.h"
#include "CollisionManager.h"
#include "CollisionSphere.h"
#include "MouseCol.h"
#include "Mouse_Manager.h"
#include "DataManager.h"
#include "GraphicDev.h"
#include "NavMgr.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager()
	:m_pDataMgr(Engine::CDataManager::GetInstance())
	, m_pMouseMgr(Engine::CMouse_Manager::GetInstance())
	, m_vPivotPos(0.f, 0.f, 0.f)
	, m_vRayDir(0.f, 0.f, 0.f)
{
	m_pDataMgr->Add_Ref();
	m_pMouseMgr->Add_Ref();
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::Initialize_CollisionMgr(void)
{
	vector<Engine::CNaviCell*> vecNavCell = Engine::CNavMgr::GetInstance()->GetVecNavMesh();

	for (_uint i = 0; i < vecNavCell.size(); ++i)
	{
		Engine::NAVMESH tNavMesh;
		ZeroMemory(&tNavMesh, sizeof(Engine::NAVMESH));

		tNavMesh.vecPoint[POINT_A] = *vecNavCell[i]->GetPoint(POINT_A);
		tNavMesh.vecPoint[POINT_B] = *vecNavCell[i]->GetPoint(POINT_B);
		tNavMesh.vecPoint[POINT_C] = *vecNavCell[i]->GetPoint(POINT_C);

		tNavMesh.dwOption = vecNavCell[i]->GetOption();
		tNavMesh.iIndex = vecNavCell[i]->GetIndex();

		if (tNavMesh.dwOption & NAV_OPTION_TOWER_ENABLE) // 타워 NavMesh
			m_vecTowerNavMesh.push_back(tNavMesh);

		m_vecNavMesh.push_back(tNavMesh);
	}
}

HRESULT CCollisionManager::Add_Component(const _tchar* pComponentTag, CComponent* pComponent)
{
	auto iter = m_MapCollisionComponent.find(pComponentTag);

	if (iter == m_MapCollisionComponent.end())
	{
		list<CComponent*> Componentlist;
		Componentlist.push_back(pComponent);
		m_MapCollisionComponent.insert(unordered_map<const _tchar*, list<CComponent*>>::value_type(pComponentTag, Componentlist));
	}
	else
	{
		iter->second.push_back(pComponent);
	}

	return NOERROR;
}

HRESULT CCollisionManager::Delete_Component(const _tchar* pComponentTag, CComponent* pComponent)
{
	auto iter = m_MapCollisionComponent.find(pComponentTag);


	for (auto iterlist = iter->second.begin(); iterlist != m_MapCollisionComponent[pComponentTag].end(); ++iterlist)
	{
		if ((*iterlist) == pComponent)
		{
			((CCollisionSphere*)(*iterlist))->Set_Life((_float)-1.f);
			iterlist = m_MapCollisionComponent[pComponentTag].erase(iterlist);
			return NOERROR;
		}
	}

	return NOERROR;
}

void CCollisionManager::Render_CollisionSphere(void)
{
	if (!m_bRender)
		return;

	for (auto iter = m_MapCollisionComponent.begin(); iter != m_MapCollisionComponent.end(); ++iter)
	{
		for (auto iterlist = iter->second.begin(); iterlist != iter->second.end(); ++iterlist)
		{
			((Engine::CCollisionSphere*)(*iterlist))->Render_CollisionSphere();
		}
	}
}

list<CComponent*> CCollisionManager::Find_Componentlist(const _tchar * pComponentTag)
{
	return m_MapCollisionComponent[pComponentTag];
}

void CCollisionManager::Add_MouseCol(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXMESH pMesh, _matrix & matWorld)
{
	// MouseCol에 정보 넘겨준다
	CMouseCol* pMouseCol = CMouseCol::Create(pGraphicDev, pMesh, matWorld);
	if (nullptr == pMouseCol)
		return;

	m_listMousecol.push_back(pMouseCol);
}

void CCollisionManager::Translation_ViewSpace(void)
{
	POINT				ptMouse = m_pMouseMgr->Get_MousePoint();

	_matrix	pmatProj = m_pDataMgr->Get_ProjMatrix();

	//x : 0	-> -1		800 -> 1
	_vec3		vTemp;
	vTemp.x = (float(ptMouse.x) / (WINCX >> 1) - 1.f) / pmatProj._11;
	vTemp.y = (float(-ptMouse.y) / (WINCY >> 1) + 1.f) / pmatProj._22;
	vTemp.z = 1.f;

	m_vPivotPos = _vec3(0.f, 0.f, 0.f);
	m_vRayDir = vTemp - m_vPivotPos;
	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
}

void CCollisionManager::Translation_Local(const _matrix * pmatWorld)
{
	// 카메라 -> 월드
	_matrix		matViewInv = m_pDataMgr->Get_ViewInverseMatrix();

	// 월드 -> 로컬
	_matrix		matWorldInv;
	D3DXMatrixInverse(&matWorldInv, nullptr, pmatWorld);

	D3DXVec3TransformCoord(&m_vPivotPos, &m_vPivotPos, &(matViewInv * matWorldInv));
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &(matViewInv * matWorldInv));
}

bool CCollisionManager::Pick_MouseCol(_vec3 & vecPick)
{
	Translation_ViewSpace();
	_matrix matIdentity;
	Translation_Local(D3DXMatrixIdentity(&matIdentity));

	return Picking(vecPick);
}

bool CCollisionManager::Pick_Terrain(const _vec3 * vecPos, const _vec3 * vecDir, _vec3 & vecPick)
{
	m_vPivotPos = *vecPos;
	m_vRayDir = *vecDir;

	return Picking(vecPick);
}

DWORD CCollisionManager::Pick_NavMesh(_vec3 & vecPick)
{
	Translation_ViewSpace();
	_matrix		matIdentity;
	Translation_Local(D3DXMatrixIdentity(&matIdentity));

	float	fU = 0.f, fV = 0.f, fDist = 0.f;
	DWORD dwOption = 0;

	float fDistance = 1000000.f;
	_vec3 vecTemp;
	_vec3 vecEye = m_pDataMgr->Get_CamPoistion();

	for (_uint i = 0; i < m_vecNavMesh.size(); ++i)
	{
		if (D3DXIntersectTri(&m_vecNavMesh[i].vecPoint[POINT_A]
			, &m_vecNavMesh[i].vecPoint[POINT_B]
			, &m_vecNavMesh[i].vecPoint[POINT_C]
			, &m_vPivotPos, &m_vRayDir, &fU, &fV, &fDist))
		{
			vecTemp = vecEye + m_vRayDir * fDist;

			// 카메라와 가장 가까운 점을 찾는다.
			if (fDistance > D3DXVec3Length(&(m_vPivotPos - vecTemp)))
			{
				dwOption = m_vecNavMesh[i].dwOption;
				fDistance = D3DXVec3Length(&(m_vPivotPos - vecTemp));
				vecPick = vecTemp;
			}
		}
	}

	return dwOption;
}

bool CCollisionManager::Picking(_vec3 & vecPick)
{
	float fDistance = 10000.f;
	bool bPick = false;

	_vec3 vecEye = m_pDataMgr->Get_CamPoistion();
	_vec3 vecTemp;

	list<CMouseCol*>::iterator iter_begin = m_listMousecol.begin();
	list<CMouseCol*>::iterator iter_end = m_listMousecol.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		if (TRUE == (*iter_begin)->PickTerrain(&m_vPivotPos, &m_vRayDir, &vecPick))
		{
			bPick = true;

			float fTemp = D3DXVec3Length(&(vecPick - vecEye)); // 카메라와 가장 가까운 점을 찾는다

			if (fTemp < fDistance)
			{
				fDistance = fTemp;
				vecTemp = vecPick;
			}
			else
				vecPick = vecTemp;
		}
	}

	return bPick;
}

void CCollisionManager::Free(void)
{
	for (auto iter = m_MapCollisionComponent.begin(); iter != m_MapCollisionComponent.end(); ++iter)
	{
		iter->second.clear();
	}
	m_MapCollisionComponent.clear();

	list<CMouseCol*>::iterator iter = m_listMousecol.begin();
	for (; iter != m_listMousecol.end(); ++iter)
		Engine::Safe_Release(*iter);

	m_listMousecol.clear();

	Engine::Safe_Release(m_pDataMgr);
	Engine::Safe_Release(m_pMouseMgr);
}
