#include "NaviCell.h"
#include "Line2D.h"
#include "GameObject.h"

Engine::CNaviCell::CNaviCell(const _vec3* pPointA
							 , const _vec3* pPointB
							 , const _vec3* pPointC)
	:m_pFontNumber(nullptr)
{
	m_vPoint[POINT_A] = *pPointA;
	m_vPoint[POINT_B] = *pPointB;
	m_vPoint[POINT_C] = *pPointC;

	ZeroMemory(m_pNeighbor, sizeof(m_pNeighbor));
	ZeroMemory(m_tPlane, sizeof(D3DXPLANE));
}

Engine::CNaviCell::~CNaviCell(void)
{
}

void Engine::CNaviCell::Set_FontNumber(CGameObject * pFontNumber)
{
	m_pFontNumber = pFontNumber;

	D3DXVECTOR3 vecPos = (m_vPoint[POINT_A] + m_vPoint[POINT_B] + m_vPoint[POINT_C]) / 3.f;
	vecPos += D3DXVECTOR3(m_tPlane.a, m_tPlane.b, m_tPlane.c); // 평면의 노말벡터만큼 살짝 이동시킨다

	m_pFontNumber->Set_Pos(&vecPos);
}

HRESULT Engine::CNaviCell::InitCell(LPDIRECT3DDEVICE9 pGraphicDev, const DWORD& dwIdx)
{
	m_pGraphicDev = pGraphicDev;
	m_dwIndex = dwIdx;

	D3DXPlaneFromPoints(&m_tPlane, &m_vPoint[POINT_A], &m_vPoint[POINT_B], &m_vPoint[POINT_C]);

	m_pLine2D[LINE_AB] = CLine2D::Create(&m_vPoint[POINT_A], &m_vPoint[POINT_B]);
	m_pLine2D[LINE_BC] = CLine2D::Create(&m_vPoint[POINT_B], &m_vPoint[POINT_C]);
	m_pLine2D[LINE_CA] = CLine2D::Create(&m_vPoint[POINT_C], &m_vPoint[POINT_A]);
	return S_OK;
}

void Engine::CNaviCell::Update_NavCell(const float & fTimeDelta)
{
	if (nullptr == m_pFontNumber)
		return;

	m_pFontNumber->Update_GameObject(fTimeDelta);
}

void Engine::CNaviCell::Render_NavCell(LPD3DXLINE pLine)
{
	D3DXMATRIX		matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	_vec3		vPoint[4];
	vPoint[0] = m_vPoint[0];
	vPoint[1] = m_vPoint[1];
	vPoint[2] = m_vPoint[2];
	vPoint[3] = m_vPoint[0];

	for(int i = 0; i < 4; ++i)
	{
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matView);
		if(vPoint[i].z < 0.f)
			vPoint[i].z = 0.f;
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matProj);
	}

	D3DXMATRIX		matIdentity;
	D3DXMatrixIdentity(&matIdentity);

	D3DXCOLOR LineColor = { 0.f, 0.f, 0.f, 1.f };

	if (m_dwOption & (DWORD)pow(2, (int)NAV_PLAYER_COLLISION))
		LineColor += D3DXCOLOR(0.5f, 0.f, 0.f, 1.f);
	if (m_dwOption & (DWORD)pow(2, (int)NAV_TOWER_ENABLE))
		LineColor += D3DXCOLOR(0.f, 0.5f, 0.f, 1.f);
	if (m_dwOption & (DWORD)pow(2, (int)NAV_WALL))
		LineColor += D3DXCOLOR(0.f, 0.f, 0.5f, 1.f);
	if (m_dwOption & (DWORD)pow(2, (int)NAV_SECOND_FLOOR))
		LineColor += D3DXCOLOR(0.5f, 0.f, 0.f, 1.f);
	if (m_dwOption & (DWORD)pow(2, (int)NAV_UPFLOOR))
		LineColor += D3DXCOLOR(0.f, 0.5f, 0.f, 1.f);
	if (m_dwOption & (DWORD)pow(2, (int)NAV_DROP))
		LineColor += D3DXCOLOR(0.f, 0.f, 0.5f, 1.f);
	if (m_dwOption & (DWORD)pow(2, (int)NAV_NOPASSAGE))
		LineColor += D3DXCOLOR(1.f, 0.f, 0.0f, 1.f);
	if (m_dwOption & (DWORD)pow(2, (int)NAV_DEATH))
		LineColor += D3DXCOLOR(1.f, 0.f, 0.0f, 1.f);

	pLine->SetWidth(1.f);
	pLine->Begin();
	pLine->DrawTransform(vPoint, POINT_END + 1, &matIdentity, LineColor);
	pLine->End();
}

bool Engine::CNaviCell::ComparePoint(const _vec3* pFristPoint, const _vec3* pSecondPoint , CNaviCell* pNeighbor)
{
	if(*pFristPoint == m_vPoint[POINT_A])
	{
		if(*pSecondPoint == m_vPoint[POINT_B])
		{
			m_pNeighbor[NEIGHBOR_AB] = pNeighbor;
			return true;
		}
		else if(*pSecondPoint == m_vPoint[POINT_C])
		{
			m_pNeighbor[NEIGHBOR_CA] = pNeighbor;
			return true;
		}
	}

	if(*pFristPoint == m_vPoint[POINT_B])
	{
		if(*pSecondPoint == m_vPoint[POINT_A])
		{
			m_pNeighbor[NEIGHBOR_AB] = pNeighbor;
			return true;
		}
		else if(*pSecondPoint == m_vPoint[POINT_C])
		{
			m_pNeighbor[NEIGHBOR_BC] = pNeighbor;
			return true;
		}
	}

	if(*pFristPoint == m_vPoint[POINT_C])
	{
		if(*pSecondPoint == m_vPoint[POINT_A])
		{
			m_pNeighbor[NEIGHBOR_CA] = pNeighbor;
			return true;
		}
		else if(*pSecondPoint == m_vPoint[POINT_B])
		{
			m_pNeighbor[NEIGHBOR_BC] = pNeighbor;
			return true;
		}
	}
	return false;
}

bool Engine::CNaviCell::CheckPass(const _vec3* pPos
			 					  , const _vec3* pDir
								  , NEIGHBORID* pNeighborID, bool& bTwoPass)
{
	int iPass = 0;
	bool bTrue = false;

	for(int i = 0; i < 3; ++i)
	{
		if(m_pLine2D[i]->CheckLinePass(&D3DXVECTOR2(pPos->x + pDir->x
			, pPos->z + pDir->z)))
		{
			iPass++;
			bTrue = true;
			*pNeighborID = NEIGHBORID(i);
		}
	}

	if (iPass == 2)
		bTwoPass = true;

	if (bTrue)
		return true;
	else
		return false;
}

bool Engine::CNaviCell::CheckNavPass(const _vec3 * pPos, const _vec3 * pDir, NEIGHBORID * pNeighborID, bool& bTwoPass)
{
	int iPass = 0;
	bool bTrue = false;

	for (int i = 0; i < 3; ++i)
	{
		if (m_pLine2D[i]->CheckLinePass(&D3DXVECTOR2(pPos->x + pDir->x
			, pPos->z + pDir->z)))
		{
			if (*pNeighborID == NEIGHBORID(i)) // 이걸 true 하는게 말이 안됨
			{
				iPass++;
			}
			else
			{
				iPass++;
				bTrue = true;
				*pNeighborID = NEIGHBORID(i);
			}
		}
	}

	if (iPass == 2)
		bTwoPass = true;
		
	if (bTrue)
		return true;
	else
		return false;
}


bool Engine::CNaviCell::CheckIn(const _vec3 * pPos)
{
	for (int i = 0; i < 3; ++i)
	{
		if (m_pLine2D[i]->CheckLinePass(&D3DXVECTOR2(pPos->x, pPos->z)))
		{
			return false;
		}
	}
	return true;
}

Engine::CNaviCell* Engine::CNaviCell::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC, const DWORD& dwIdx)
{
	CNaviCell*	pCell = new CNaviCell(pPointA, pPointB, pPointC);
	if (FAILED(pCell->InitCell(pGraphicDev, dwIdx)))
		Safe_Release(pCell);

	return pCell;
}

void Engine::CNaviCell::Free(void)
{
	for (int i = 0; i < LINE_END; ++i)
		Safe_Release(m_pLine2D[i]);

	Safe_Release(m_pFontNumber);
}