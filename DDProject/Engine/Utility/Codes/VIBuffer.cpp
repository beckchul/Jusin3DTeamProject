#include "VIBuffer.h"

USING(Engine)

Engine::CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
: CResources(pGraphicDev)
, m_pVB(nullptr)
, m_pIB(nullptr)
, m_dwVtxCnt(0)
, m_dwVtxSize(0)
, m_dwVtxFVF(0)
{
	
}

Engine::CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
: CResources(rhs)
, m_pVB(rhs.m_pVB)
, m_pIB(rhs.m_pIB)
, m_dwVtxCnt(rhs.m_dwVtxCnt)
, m_dwVtxSize(rhs.m_dwVtxSize)
, m_dwVtxFVF(rhs.m_dwVtxFVF)
, m_dwIdxSize(rhs.m_dwIdxSize)
, m_dwTriCnt(rhs.m_dwTriCnt)
, m_IndexFmt(rhs.m_IndexFmt)
{
	if(m_pVB != nullptr)
		m_pVB->AddRef();

	if (m_pIB != nullptr)
		m_pIB->AddRef();
}

Engine::CVIBuffer::~CVIBuffer(void)
{

}

void CVIBuffer::GetVtxInfo(void * pVtxInfo)
{
	void* pOriVtx = nullptr;

	m_pVB->Lock(0, 0, (void**)&pOriVtx, 0);
	memcpy(pVtxInfo, pOriVtx, m_dwVtxSize * m_dwVtxCnt);
	m_pVB->Unlock();
}

void CVIBuffer::SetVtxInfo(void * pVtxInfo)
{
	void* pOriVtx = nullptr;
	m_pVB->Lock(0, 0, (void**)&pOriVtx, 0);
	memcpy(pOriVtx, pVtxInfo, m_dwVtxSize * m_dwVtxCnt);
	m_pVB->Unlock();
}

void CVIBuffer::SetIdxInfo(void * pIdxInfo, const DWORD * pTriCnt)
{
	void* pOriIndex = nullptr;

	m_dwTriCnt = *pTriCnt;

	m_pIB->Lock(0, 0, &pOriIndex, 0);
	memcpy(pOriIndex, pIdxInfo, m_dwIdxSize * m_dwTriCnt);
	m_pIB->Unlock();
}

HRESULT Engine::CVIBuffer::Ready_Buffer(void)
{
	// CreateVertexBuffer : 정점(Vertex)들을 보관하기위한 메모리공간(Buffer)을 생성한다.
	if(FAILED(m_pGraphicDev->CreateVertexBuffer(m_dwVtxSize * m_dwVtxCnt, 0, m_dwVtxFVF, D3DPOOL_MANAGED, &m_pVB, NULL)))
		return E_FAIL;
	if(FAILED(m_pGraphicDev->CreateIndexBuffer(m_dwIdxSize, 0, m_IndexFmt, D3DPOOL_MANAGED, &m_pIB, NULL)))
		return E_FAIL;
		

	return S_OK;
}

void Engine::CVIBuffer::Free(void)
{
	Engine::Safe_Release(m_pVB);
	Engine::Safe_Release(m_pIB);

	CResources::Free();
}
