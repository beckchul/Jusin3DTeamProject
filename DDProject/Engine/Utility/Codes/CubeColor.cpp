#include "CubeColor.h"
#include "Mesh.h"
#include "Static_Mesh.h"

USING(Engine)

CCubeColor::CCubeColor(LPDIRECT3DDEVICE9 pGraphicDev)
: Engine::CVIBuffer(pGraphicDev)
{

}

CCubeColor::CCubeColor(const CCubeColor& rhs)
: Engine::CVIBuffer(rhs)
{

}

CCubeColor::~CCubeColor(void)
{

}

void CCubeColor::Set_ColInfo(CMesh* pMesh)
{
	((CStaticMesh*)pMesh)->Compute_MinMax(&m_vMin, &m_vMax);
	SetBoundingBox(&m_vMin, &m_vMax, D3DCOLOR_ARGB(255, 0, 255, 0));
}

void CCubeColor::SetBoundingBox(const _vec3* pMin, const _vec3* pMax, const DWORD& dwColor)
{
	VTXCOL*		pVertex = nullptr;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// setting BoundingBox Vertex position
	{
		pVertex[0].vPos = _vec3(pMin->x, pMax->y, pMin->z);
		pVertex[1].vPos = _vec3(pMax->x, pMax->y, pMin->z);
		pVertex[2].vPos = _vec3(pMax->x, pMin->y, pMin->z);
		pVertex[3].vPos = _vec3(pMin->x, pMin->y, pMin->z);

		pVertex[4].vPos = _vec3(pMin->x, pMax->y, pMax->z);
		pVertex[5].vPos = _vec3(pMax->x, pMax->y, pMax->z);
		pVertex[6].vPos = _vec3(pMax->x, pMin->y, pMax->z);
		pVertex[7].vPos = _vec3(pMin->x, pMin->y, pMax->z);
	}

	for (DWORD i = 0; i < m_dwVtxCnt; ++i)
		pVertex[i].dwColor = dwColor;

	m_pVB->Unlock();
}



HRESULT CCubeColor::Ready_Buffer(const _vec3* pMin, const _vec3* pMax, const DWORD& dwColor)
{
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 8;
	m_dwVtxFVF = VTXFVF_COL;

	m_dwTriCnt = 12;
	m_dwIdxSize = sizeof(INDEX16) * m_dwTriCnt;;
	m_IndexFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXCOL*			pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = _vec3(pMin->x, pMax->y, pMin->z);
	pVertex[0].dwColor = dwColor;

	pVertex[1].vPos = _vec3(pMax->x, pMax->y, pMin->z);
	pVertex[1].dwColor = dwColor;

	pVertex[2].vPos = _vec3(pMax->x, pMin->y, pMin->z);
	pVertex[2].dwColor = dwColor;

	pVertex[3].vPos = _vec3(pMin->x, pMin->y, pMin->z);
	pVertex[3].dwColor = dwColor;


	pVertex[4].vPos = _vec3(pMin->x, pMax->y, pMax->z);
	pVertex[4].dwColor = dwColor;

	pVertex[5].vPos = _vec3(pMax->x, pMax->y, pMax->z);
	pVertex[5].dwColor = dwColor;

	pVertex[6].vPos = _vec3(pMax->x, pMin->y, pMax->z);
	pVertex[6].dwColor = dwColor;

	pVertex[7].vPos = _vec3(pMin->x, pMin->y, pMax->z);
	pVertex[7].dwColor = dwColor;

	m_pVB->Unlock();


	INDEX16*			pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// +x
	pIndex[0]._0 = 1; pIndex[0]._1 = 5; pIndex[0]._2 = 6;
	pIndex[1]._0 = 1; pIndex[1]._1 = 6; pIndex[1]._2 = 2;

	// -x									   
	pIndex[2]._0 = 4; pIndex[2]._1 = 0; pIndex[2]._2 = 3;
	pIndex[3]._0 = 4; pIndex[3]._1 = 3; pIndex[3]._2 = 7;


	// +y									   
	pIndex[4]._0 = 4; pIndex[4]._1 = 5; pIndex[4]._2 = 1;
	pIndex[5]._0 = 4; pIndex[5]._1 = 1; pIndex[5]._2 = 0;

	// -y									   
	pIndex[6]._0 = 3; pIndex[6]._1 = 2; pIndex[6]._2 = 6;
	pIndex[7]._0 = 3; pIndex[7]._1 = 6; pIndex[7]._2 = 7;


	// +z									   
	pIndex[8]._0 = 7; pIndex[8]._1 = 6; pIndex[8]._2 = 5;
	pIndex[9]._0 = 7; pIndex[9]._1 = 5; pIndex[9]._2 = 4;

	// -z
	pIndex[10]._0 = 0; pIndex[10]._1 = 1; pIndex[10]._2 = 2;
	pIndex[11]._0 = 0; pIndex[11]._1 = 2; pIndex[11]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void CCubeColor::Render_Buffer(const _matrix* pmatWorld, const DWORD& dwColor)
{
	SetBoundingBox(&m_vMin, &m_vMax, dwColor);

	m_pGraphicDev->SetTexture(0, nullptr);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	if (pmatWorld != NULL)
		m_pGraphicDev->SetTransform(D3DTS_WORLD, pmatWorld);

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pGraphicDev->SetFVF(m_dwVtxFVF);
	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

CCubeColor* CCubeColor::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pMin, const _vec3* pMax, const DWORD& dwColor)
{
	CCubeColor*		pBuffer = new CCubeColor(pGraphicDev);
	if (FAILED(pBuffer->Ready_Buffer(pMin, pMax, dwColor)))
	{
		MSG_BOX(L"CubeColor Created Failed");
		pBuffer->Release();
	}

	return pBuffer;
}

CComponent* CCubeColor::Clone(void)
{
	return new CCubeColor(*this);
}

void CCubeColor::Free(void)
{
	CVIBuffer::Free();
}