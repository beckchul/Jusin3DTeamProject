#include "NavMgr.h"
#include "NaviCell.h"
#include "Line2D.h"
#include "Transform.h"
#include "InputDev.h"

IMPLEMENT_SINGLETON(Engine::CNavMgr)

Engine::CNavMgr::CNavMgr(void)
: m_pGraphicDev(nullptr)
, m_dwReserveSize(0)
, m_dwIdxCnt(0)
, m_pLine(nullptr)
{

}

Engine::CNavMgr::~CNavMgr(void)
{
}

void Engine::CNavMgr::ReserveCellSize(LPDIRECT3DDEVICE9 pGraphicDev, const DWORD& dwSize)
{
	m_pGraphicDev = pGraphicDev;
	m_dwReserveSize = dwSize;
	m_vecNaviMesh.reserve(dwSize);

	if (FAILED(D3DXCreateLine(m_pGraphicDev, &m_pLine)))
		return;
}

HRESULT Engine::CNavMgr::AddCell(NAVMESH* pNavMesh, CGameObject* pFontNumber)
{
	CNaviCell*		pCell = CNaviCell::Create(m_pGraphicDev,
		&pNavMesh->vecPoint[POINT_A], &pNavMesh->vecPoint[POINT_B], &pNavMesh->vecPoint[POINT_C], pNavMesh->iIndex);

	if (nullptr == pCell)
		return E_FAIL;

	pCell->SetOption(pNavMesh->dwOption);
	pCell->Set_FontNumber(pFontNumber);

	++m_dwIdxCnt;

	m_vecNaviMesh.push_back(pCell);

	if (pNavMesh->dwOption & NAV_OPTION_SECOND_FLOOR) // 2층 Cell
		m_vecNaviMeshSecondFloor.push_back(pCell);

	return S_OK;
}

void Engine::CNavMgr::LinkCell(void)
{
	VECCELL::iterator	iter = m_vecNaviMesh.begin();
	if(iter == m_vecNaviMesh.end())
		return;

	for( ; iter != m_vecNaviMesh.end(); ++iter)
	{
		VECCELL::iterator	iter_Target = m_vecNaviMesh.begin();
		while(iter_Target != m_vecNaviMesh.end())
		{
			if(iter == iter_Target)
			{
				++iter_Target;
				continue;
			}

			if((*iter_Target)->ComparePoint((*iter)->GetPoint(POINT_A), (*iter)->GetPoint(POINT_B)
				, (*iter)))
			{
				(*iter)->SetNeighbor(NEIGHBOR_AB, (*iter_Target));
			}
			else if((*iter_Target)->ComparePoint((*iter)->GetPoint(POINT_B), (*iter)->GetPoint(POINT_C)
				, (*iter)))
			{
				(*iter)->SetNeighbor(NEIGHBOR_BC, (*iter_Target));
			}
			else if((*iter_Target)->ComparePoint((*iter)->GetPoint(POINT_C), (*iter)->GetPoint(POINT_A)
				, (*iter)))
			{
				(*iter)->SetNeighbor(NEIGHBOR_CA, (*iter_Target));
			}
			++iter_Target;
		}
	}
}

void Engine::CNavMgr::Reset_Cell(void)
{
	for_each(m_vecNaviMesh.begin(), m_vecNaviMesh.end(), CRelease_Single());
	m_vecNaviMesh.clear();

	m_vecNaviMeshSecondFloor.clear();
}

void Engine::CNavMgr::Update_NavMesh(const float& fTimeDelta)
{
	if (CInputDev::GetInstance()->OnceKeyDown(DIK_N))
		m_eShowNavMesh = eShowNavMesh((m_eShowNavMesh + 1) % 3);

	if (SHOW_NAVMESH_FONT != m_eShowNavMesh)
		return;

	UINT		iSize = m_vecNaviMesh.size();
	for (UINT i = 0; i < iSize; ++i)
		m_vecNaviMesh[i]->Update_NavCell(fTimeDelta);
}

void Engine::CNavMgr::Render_NavMesh(void)
{
	if (SHOW_NOTHING == m_eShowNavMesh)
		return;

	UINT		iSize = m_vecNaviMesh.size();
	for(UINT i = 0; i < iSize; ++i)
		m_vecNaviMesh[i]->Render_NavCell(m_pLine);
}

DWORD Engine::CNavMgr::MoveOnNavMesh(_vec3 * pPos, const _vec3 * pDir, const DWORD & dwCurrentIdx, bool bDropAble)
{
	NEIGHBORID		eNeighborID;
	DWORD			dwNextIndex = dwCurrentIdx;

	if (-1 == dwCurrentIdx)
	{
		*pPos += *pDir;
		return -1;
	}

	bool bTwoPass = false;
	if (m_vecNaviMesh[dwCurrentIdx]->CheckPass(pPos, pDir, &eNeighborID, bTwoPass))
	{
		CNaviCell*   pNeighbor = m_vecNaviMesh[dwCurrentIdx]->GetNeighbor(eNeighborID);

		if (nullptr == pNeighbor) //존재하지 않는 이웃
		{
			DWORD dwOption = m_vecNaviMesh[dwCurrentIdx]->GetOption();

			if (dwOption & NAV_OPTION_DROP)
			{
				if(bDropAble) // 떨어질 수 있는애들만
					Drop(pPos, pDir, dwNextIndex);
			}
			else  // 슬라이딩 벡터
				Slide_Vector(pPos, pDir, dwNextIndex, eNeighborID);
		}
		else // 존재하는 이웃
		{
			// 통행 불가
			if ((m_vecNaviMesh[dwNextIndex]->GetOption() & NAV_OPTION_NOPASSAGE) == FALSE &&
				pNeighbor->GetOption() & NAV_OPTION_NOPASSAGE) 
			{
				Slide_Vector(pPos, pDir, dwNextIndex, eNeighborID);
				return dwNextIndex;
			}

			if (FALSE == bTwoPass)
				*pPos += *pDir;

			dwNextIndex = pNeighbor->GetIndex();
		}
	}
	else // Cell을 넘어가지 않는 경우
	{
		if (FALSE == bTwoPass)
			*pPos += *pDir;
	}

	return dwNextIndex;
}

bool Engine::CNavMgr::FallOnNavMesh(CTransform* pTransform, const DWORD & dwCurrentIdx, bool& bDead)
{
	bDead = false;

	if (-1 == dwCurrentIdx)
		return false;

	D3DXPLANE Plane = m_vecNaviMesh[dwCurrentIdx]->GetPlane();

	_vec3 vecPos = (pTransform)->Get_Info(CTransform::INFO_POSITION);

	float fY = (-Plane.a * vecPos.x - Plane.c * vecPos.z - Plane.d) / Plane.b;

	if (vecPos.y <= fY) // 플레이어의 위치가 더 아래
	{
		vecPos.y = fY;
		(pTransform)->Set_Information(CTransform::INFO_POSITION, &vecPos);

		DWORD dwOption = m_vecNaviMesh[dwCurrentIdx]->GetOption();
		if (dwOption & NAV_OPTION_DEATH) // 죽는 내비메쉬
			bDead = true;

		return true;
	}

	return false;
}

DWORD Engine::CNavMgr::FindNavMesh(_vec3& vPos)
{
	if (m_vecNaviMesh.size() == 0)
		return -1;

	float fDistance = 1000000.f; // 
	float fTemp = 0.f;
	DWORD dwIndex = -1;

	for (size_t i=0; i<m_vecNaviMesh.size(); ++i)
	{
		D3DXPLANE Plane = m_vecNaviMesh[i]->GetPlane();

		// 1차 조건 : 평면보다 위에 있을 것
		
		fTemp = Plane.a * vPos.x + Plane.b * vPos.y + Plane.c * vPos.z + Plane.d;
		if (fTemp < 0.f || fTemp > 10.f) // 너무 밑에 있는것도 X
			continue;

		// 2차 조건 : 유한한 평면 위의 점일 것

		if (!m_vecNaviMesh[i]->CheckIn(&vPos))
			continue;

		if (fTemp < fDistance) // 가장 가까운 거리를 찾기 위함
		{
			fDistance = fTemp;
			dwIndex = m_vecNaviMesh[i]->GetIndex();
		}
			
		//return dwIndex;
	}

	return dwIndex;
}

DWORD Engine::CNavMgr::FindSecondFloor(_vec3 & vPos, const DWORD dwCurrentIdx)
{
	float fDistance = 1000000.f;
	float fTemp = 0.f;
	DWORD dwIndex = dwCurrentIdx;

	for (size_t i =  0; i < m_vecNaviMeshSecondFloor.size(); ++i)
	{
		if (i == dwCurrentIdx)
			continue;

		D3DXPLANE Plane = m_vecNaviMeshSecondFloor[i]->GetPlane();

		// 1차 조건 : 평면보다 위에 있을 것
		fTemp = Plane.a * vPos.x + Plane.b * vPos.y + Plane.c * vPos.z + Plane.d;
		if (fTemp < 0)
			continue;

		// 2차 조건 : 유한한 평면 위의 점일 것
		if (!m_vecNaviMeshSecondFloor[i]->CheckIn(&vPos))
			continue;

		if (fTemp < fDistance) // 가장 가까운 거리를 찾기 위함
		{
			fDistance = fTemp;
			dwIndex = m_vecNaviMeshSecondFloor[i]->GetIndex();
		}

		//return m_vecNaviMeshSecondFloor[i]->GetIndex();
	}

	return dwIndex;
}

Engine::POINTID Engine::CNavMgr::Get_NearestPoint(CNaviCell* pNavCell, _vec3 vPoint)
{
	POINTID ePoint = POINT_END;
	float fDistance = 10000.f;

	for (int i = 0; i < POINT_END; ++i)
	{
		float fTemp = D3DXVec3Length(&(*pNavCell->GetPoint(POINTID(i)) - vPoint));

		if (fTemp < fDistance)
		{
			fDistance = fTemp;
			ePoint = POINTID(i);
		}
	}

	return ePoint;
}

Engine::CNaviCell * Engine::CNavMgr::Get_NearNeighbor(CNaviCell * pNavCell, POINTID ePoint)
{
	CNaviCell* pNeighbor = nullptr;

	switch (ePoint)
	{
	case POINT_A:
		pNeighbor = pNavCell->GetNeighbor(NEIGHBOR_AB);
		if (nullptr != pNeighbor)
			return pNeighbor;
		pNeighbor = pNavCell->GetNeighbor(NEIGHBOR_CA);
		if (nullptr != pNeighbor)
			return pNeighbor;
		break;

	case POINT_B:
		pNeighbor = pNavCell->GetNeighbor(NEIGHBOR_AB);
		if (nullptr != pNeighbor)
			return pNeighbor;
		pNeighbor = pNavCell->GetNeighbor(NEIGHBOR_BC);
		if (nullptr != pNeighbor)
			return pNeighbor;
		break;

	case POINT_C:
		pNeighbor = pNavCell->GetNeighbor(NEIGHBOR_BC);
		if (nullptr != pNeighbor)
			return pNeighbor;
		pNeighbor = pNavCell->GetNeighbor(NEIGHBOR_CA);
		if (nullptr != pNeighbor)
			return pNeighbor;
		break;
	}

	return nullptr;
}

bool Engine::CNavMgr::IsAbleToUpStairs(const DWORD dwCurrentIdx)
{
	DWORD dwOption = m_vecNaviMesh[dwCurrentIdx]->GetOption();

	if (dwOption & NAV_OPTION_UPFLOOR)
		return true;

	return false;
}

void Engine::CNavMgr::Slide_Vector(_vec3 * pPos, const _vec3 * pDir, DWORD & dwIndex, NEIGHBORID eNeighborID)
{
	// 슬라이드 벡터 계산
	_vec2 vec2Normal = m_vecNaviMesh[dwIndex]->GetLine(LINE(eNeighborID))->GetNormal();
	_vec3 vec3Normal = { vec2Normal.x, 0.f, vec2Normal.y };
	_vec3 vecSlide = *pDir - vec3Normal * (D3DXVec3Dot(&vec3Normal, pDir));

	bool bTwoPass = false;
	if (m_vecNaviMesh[dwIndex]->CheckNavPass(pPos, &vecSlide, &eNeighborID, bTwoPass)) // 슬라이딩 벡터로 삼각형을 벗어나는 경우
	{
		CNaviCell*   pNeighbor = m_vecNaviMesh[dwIndex]->GetNeighbor(eNeighborID);

		if (nullptr != pNeighbor) // 이웃이 있는 경우
		{
			// 통행 불가
			if ((m_vecNaviMesh[dwIndex]->GetOption() & NAV_OPTION_NOPASSAGE) == FALSE &&
				pNeighbor->GetOption() & NAV_OPTION_NOPASSAGE) 
				return;

			dwIndex = pNeighbor->GetIndex();

			if (FALSE == bTwoPass)
				*pPos += vecSlide;
		}
		else
		{
			//cout << "낑김.." << endl;
		}
			
	}
	else // 슬라이딩 벡터로 그냥 이동
	{
		if(FALSE == bTwoPass)
			*pPos += vecSlide;
	}
}

void Engine::CNavMgr::Drop(_vec3 * pPos, const _vec3 * pDir, DWORD & dwIndex)
{
	dwIndex = -1;
	*pPos += *pDir;
}

void Engine::CNavMgr::Free(void)
{
	for_each(m_vecNaviMesh.begin(), m_vecNaviMesh.end(), CRelease_Single());
	m_vecNaviMesh.clear();

	Safe_Release(m_pLine);
}