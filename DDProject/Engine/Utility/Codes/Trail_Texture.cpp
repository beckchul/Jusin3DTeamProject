#include "Trail_Texture.h"

USING(Engine)

Engine::CTrail_Texture::CTrail_Texture(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CVIBuffer(pGraphicDev)
	, m_pVertex(nullptr)
{

}
Engine::CTrail_Texture::CTrail_Texture(const CTrail_Texture& rhs)
	: Engine::CVIBuffer(rhs)
	, m_pVertex(rhs.m_pVertex)
{

}

Engine::CTrail_Texture::~CTrail_Texture(void)
{

}

HRESULT Engine::CTrail_Texture::Ready_Buffer(void)
{
	m_dwVtxCnt = 0;
	m_dwVtxSize = sizeof(VTXTRAIL);
	m_dwVtxFVF = D3DFVF_VTXTRAIL;

	m_dwTriCnt = 0;

	return S_OK;
}

void Engine::CTrail_Texture::Render_Buffer(void)
{
	m_pGraphicDev->SetFVF(m_dwVtxFVF);
	m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, m_dwTriCnt, m_pVertex, m_dwVtxSize);

	Safe_Delete_Array(m_pVertex);
}

void CTrail_Texture::SetVertexTrail(const _vec3 * pPos, const DWORD & dwCnt)
{
	m_pVertex = new VTXTRAIL[dwCnt];
	ZeroMemory(m_pVertex, sizeof(VTXTRAIL) * dwCnt);

	for (DWORD i = 0; i < dwCnt; ++i)
	{
		m_pVertex[i].vPos = pPos[i];

		if (i % 2)
			m_pVertex[i].vTexUV = D3DXVECTOR2(1 / (dwCnt - 1.f), 0.f);
		else
			m_pVertex[i].vTexUV = D3DXVECTOR2((i - 1) / (dwCnt - 1.f), 1.f);
	}

	m_dwTriCnt = dwCnt - 2;
}

CTrail_Texture* Engine::CTrail_Texture::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrail_Texture*		pInstance = new CTrail_Texture(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		MSG_BOX(L"CTrail_Texture Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* Engine::CTrail_Texture::Clone(void)
{
	return new CTrail_Texture(*this);
}

void Engine::CTrail_Texture::Free(void)
{
	Safe_Delete_Array(m_pVertex);

	Engine::CVIBuffer::Free();
}

