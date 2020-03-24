#include "View_Texture.h"

USING(Engine)

Engine::CView_Texture::CView_Texture(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CVIBuffer(pGraphicDev)
{

}
Engine::CView_Texture::CView_Texture(const CView_Texture& rhs)
	: Engine::CVIBuffer(rhs)
{

}

Engine::CView_Texture::~CView_Texture(void)
{

}

HRESULT Engine::CView_Texture::Ready_Buffer(_vec4* pBufferInfo)
{
	m_dwVtxCnt = 4;
	m_dwVtxSize = sizeof(VTXVIEWTEX);
	m_dwVtxFVF = D3DFVF_VTXVIEWTEX;

	m_dwTriCnt = 2;
	m_dwIdxSize = sizeof(INDEX16) * m_dwTriCnt;
	m_IndexFmt = D3DFMT_INDEX16;

	if (FAILED(Engine::CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	// 할당된 메모리에 접근을 하자.
	VTXVIEWTEX*				pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// -0.5 보정이 필요할수도
	pVertex[0].vPosition = _vec4(pBufferInfo->x - 0.5f, pBufferInfo->y - 0.5f, 0.f, 1.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);

	pVertex[1].vPosition = _vec4(pBufferInfo->x + pBufferInfo->z - 0.5f, pBufferInfo->y - 0.5f, 0.f, 1.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPosition = _vec4(pBufferInfo->x + pBufferInfo->z - 0.5f, pBufferInfo->y + pBufferInfo->w - 0.5f, 0.f, 1.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPosition = _vec4(pBufferInfo->x - 0.5f, pBufferInfo->y + pBufferInfo->w - 0.5f, 0.f, 1.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);

	m_pVB->Unlock();

	INDEX16*			pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void Engine::CView_Texture::Render_Buffer(void)
{
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pGraphicDev->SetFVF(D3DFVF_VTXVIEWTEX);
	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
}

CView_Texture* Engine::CView_Texture::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec4* pBufferInfo)
{
	CView_Texture*		pInstance = new CView_Texture(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(pBufferInfo)))
	{
		MSG_BOX(L"CView_Texture Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* Engine::CView_Texture::Clone(void)
{
	return new CView_Texture(*this);
}

void Engine::CView_Texture::Free(void)
{
	Engine::CVIBuffer::Free();
}

