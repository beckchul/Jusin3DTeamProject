#include "Particle_Texture.h"

USING(Engine)

Engine::CParticle_Texture::CParticle_Texture(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CVIBuffer(pGraphicDev)
	, m_pVertex(nullptr)
{

}
Engine::CParticle_Texture::CParticle_Texture(const CParticle_Texture& rhs)
	: Engine::CVIBuffer(rhs)
{

}

Engine::CParticle_Texture::~CParticle_Texture(void)
{

}

HRESULT Engine::CParticle_Texture::Ready_Buffer(void)
{
	m_dwVtxCnt = 2048;
	m_dwVtxSize = sizeof(VTXPARTICLE);
	m_dwVtxFVF = D3DFVF_VTXPARTICLETEX;

	m_dwTriCnt = 0;

	//////////////////////////////////////////////////////////
	//m_dwVbSize = 2048;
	//m_dwVbOffset = 0;
	//m_dwVbBatchSize = 512;

	//m_pGraphicDev->CreateVertexBuffer(
	//	m_dwVbSize * sizeof(Engine::VTXPARTICLE)
	//	, D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
	//	, Engine::D3DFVF_VTXPARTICLETEX
	//	, D3DPOOL_DEFAULT
	//	, &m_pVb
	//	, 0);

	if (FAILED(m_pGraphicDev->CreateVertexBuffer(
		m_dwVtxCnt * m_dwVtxSize
		, D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
		, m_dwVtxFVF
		, D3DPOOL_DEFAULT
		, &m_pVB
		, NULL)))
		return E_FAIL;

	return S_OK;
}

void Engine::CParticle_Texture::Render_Buffer(void)
{
	//m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, m_dwTriCnt, m_pVertex, m_dwVtxSize);
	
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXPARTICLE));
	m_pGraphicDev->SetFVF(m_dwVtxFVF);

	//m_pGraphicDev->DrawPrimitive(
	//	D3DPT_POINTLIST,
	//	m_dwVbOffset,
	//	numParticlesInBatch);

}


CParticle_Texture* Engine::CParticle_Texture::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CParticle_Texture*		pInstance = new CParticle_Texture(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		MSG_BOX(L"CParticle_Texture Created Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* Engine::CParticle_Texture::Clone(void)
{
	return new CParticle_Texture(*this);
}

void Engine::CParticle_Texture::Free(void)
{

	Engine::CVIBuffer::Free();
}

