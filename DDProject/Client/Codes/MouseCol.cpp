#include "stdafx.h"
#include "MouseCol.h"
#include "DataManager.h"
#include "NavMgr.h"

CMouseCol::CMouseCol(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCollision(pGraphicDev)
	, m_pVertex(nullptr)
	, m_pIndex(nullptr)
	, m_pCloneMesh(nullptr)
	, m_pDataMgr(CDataManager::GetInstance())
{
	m_pDataMgr->Add_Ref();
}

CMouseCol::~CMouseCol(void)
{

}

HRESULT CMouseCol::Initialize(LPD3DXMESH pMesh, _matrix& matWorld)
{
	// 복사
	pMesh->CloneMeshFVF(D3DXMESH_MANAGED, Engine::D3DFVF_VTXPOS, m_pGraphicDev, &m_pCloneMesh);

	m_pCloneMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&m_pVertex); // Lock
	m_pCloneMesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&m_pIndex);

	// m_pVertex(정점)의 정보를 바꿔준다
	for (unsigned int i = 0; i < pMesh->GetNumVertices(); ++i)
		D3DXVec3TransformCoord(&m_pVertex[i].vPos, &m_pVertex[i].vPos, &matWorld);

	m_pCloneMesh->UnlockVertexBuffer(); // UnLock
	m_pCloneMesh->UnlockIndexBuffer();

	m_iFaceNum = pMesh->GetNumFaces();

	return S_OK;
}

bool CMouseCol::PickTerrain(_vec3* vecPos, _vec3* vecDir, _vec3* pOut)
{
	float	fU = 0.f, fV = 0.f, fDist = 0.f;
	bool bPick = false;

	float fDistance = 10000.f;
	_vec3 vecTemp;
	_vec3 vecEye = m_pDataMgr->Get_CamPoistion();

	for (int i = 0; i < m_iFaceNum; ++i)
	{
		if (D3DXIntersectTri(&(m_pVertex[m_pIndex[i]._0].vPos)
			, &(m_pVertex[m_pIndex[i]._1].vPos)
			, &(m_pVertex[m_pIndex[i]._2].vPos)
			, vecPos, vecDir, &fU, &fV, &fDist))
		{
			/*vecTemp = (m_pVertex[m_pIndex[i]._0].vPos
				+ (m_pVertex[m_pIndex[i]._1].vPos - m_pVertex[m_pIndex[i]._0].vPos) * fU
				+ (m_pVertex[m_pIndex[i]._2].vPos - m_pVertex[m_pIndex[i]._0].vPos) * fV);*/

			vecTemp = *vecPos + *vecDir * fDist;

			// 카메라와 가장 가까운 점을 찾는다.
			if (fDistance > D3DXVec3Length(&(*vecPos - vecTemp)))
			{
				bPick = true;

				fDistance = D3DXVec3Length(&(*vecPos - vecTemp));
				*pOut = vecTemp;
			}
		}
	}

	return bPick;
}

CComponent * CMouseCol::Clone(void)
{
	return new CMouseCol(*this);
}

CMouseCol* CMouseCol::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXMESH pMesh, _matrix& matWorld)
{
	CMouseCol*		pCollision = new CMouseCol(pGraphicDev);
	if (FAILED(pCollision->Initialize(pMesh, matWorld)))
		Engine::Safe_Release(pCollision);

	return pCollision;
}

void CMouseCol::Free(void)
{
	Engine::Safe_Release(m_pCloneMesh);
	Engine::Safe_Release(m_pDataMgr);

	CCollision::Free();
}