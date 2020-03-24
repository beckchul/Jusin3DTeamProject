#include "MediaMgr.h"

#include "MediaObj.h"
#include "Rect_Texture.h"

USING(Engine)

IMPLEMENT_SINGLETON(CMediaMgr)

Engine::CMediaMgr::CMediaMgr(void)
: m_pMediaObj(nullptr)
, m_pRcTexBuffer(nullptr)
, m_pGraphicDev(nullptr)
, m_fWidth(0.f)
, m_fHeight(0.f)
, m_pVtxPos(nullptr)
, m_pVtxConPos(nullptr)
{
}

Engine::CMediaMgr::~CMediaMgr(void)
{
}

HRESULT Engine::CMediaMgr::VideoPlay(WCHAR* wFileName)
{
	if (nullptr == m_pMediaObj) // 처음 비디오 재생
	{
		m_pMediaObj = new CMediaObj;
		m_pMediaObj->Initialize(m_pGraphicDev, wFileName);
	}
	else // 이미 재생한적 있는 경우
	{
		DWORD dwRefCnt = Engine::Safe_Release(m_pMediaObj);

		m_pMediaObj = new CMediaObj;
		m_pMediaObj->Initialize(m_pGraphicDev, wFileName);
	}

	return S_OK;
}

HRESULT Engine::CMediaMgr::StopPlay(void)
{
	Engine::Safe_Release(m_pMediaObj);

	return NOERROR;
}

void Engine::CMediaMgr::Ready_MediaMgr(LPDIRECT3DDEVICE9 pGraphicDev, float fWidth, float fHeight)
{
	m_pGraphicDev = pGraphicDev;
	m_pGraphicDev->AddRef();

	m_fWidth = fWidth;
	m_fHeight = fHeight;

	CoInitialize(NULL);

	InitMoviePos();

	m_pRcTexBuffer = CRect_Texture::Create(m_pGraphicDev);
}

void Engine::CMediaMgr::Progress(void)
{
	VertexRenewal();
}

void Engine::CMediaMgr::Render(void)
{
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	// view, projection 행렬이 적용 안되어있을 때에만 전체 화면으로 나온다
	D3DXMATRIX		matTmp, matOldView, matOldProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, D3DXMatrixIdentity(&matTmp));
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, D3DXMatrixIdentity(&matTmp));

	m_pMediaObj->SetTexMovie(0);
	m_pRcTexBuffer->SetVtxInfo(m_pVtxConPos);
	m_pRcTexBuffer->Render_Buffer();

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
}

void Engine::CMediaMgr::InitMoviePos()
{
	m_vVanPoint = _vec3(m_fWidth / 2.f, 100.0f, 500.0f);

	m_vPos = _vec3(m_fWidth / 2.f, m_fHeight / 2.f, 0.0f);
	m_vPos.z = m_vVanPoint.z;
	m_fHalfWidth = m_fWidth / 2.f;
	m_fHalfHeight = m_fHeight / 2.f;

	m_pVtxPos = new Engine::VTXTEX[4];
	m_pVtxConPos = new Engine::VTXTEX[4];

	for(int i = 0; i < 4; ++i)
	{
		m_pVtxPos[i].vPosition = _vec3(m_vPos.x + (i % 3 == 0 ? -m_fHalfWidth : m_fHalfWidth), 
			m_vPos.y + (i < 2 ? -m_fHalfHeight : m_fHalfHeight), 
			m_vPos.z);
		m_pVtxPos[i].vTexUV = D3DXVECTOR2((i % 3 == 0 ? 0.0f : 1.0f), 
			(i < 2 ? 0.0f : 1.0f));
	}
}

void Engine::CMediaMgr::VertexRenewal(void)
{
	if(m_pVtxPos == nullptr || m_pVtxConPos == nullptr)
		return;

	for(int i = 0; i < 4; ++i)
	{
		m_pVtxConPos[i] = m_pVtxPos[i];

		ProjVector(m_pVtxConPos[i].vPosition, m_vVanPoint);

		ConvertVector(m_pVtxConPos[i].vPosition, (int)m_fWidth, (int)m_fHeight);

		m_pVtxConPos[i].vPosition.z = 0.0f;
	}
}

void Engine::CMediaMgr::ProjVector(_vec3& Vector, const _vec3& VenPoint)
{
	Vector.x = VenPoint.x + (Vector.x - VenPoint.x) * (VenPoint.z / Vector.z);
	Vector.y = VenPoint.y + (Vector.y - VenPoint.y) * (VenPoint.z / Vector.z);
}

void Engine::CMediaMgr::ConvertVector(_vec3& Vector, int iWidth, int iHeight)
{
	Vector.x = (Vector.x * 2.0f / iWidth) - 1.0f;
	Vector.y = -((Vector.y * 2.0f / iHeight) - 1.0f);
}

void CMediaMgr::Free(void)
{
	Engine::Safe_Release(m_pGraphicDev);

	Engine::Safe_Release(m_pRcTexBuffer);
	Engine::Safe_Release(m_pMediaObj);
	Engine::Safe_Delete_Array(m_pVtxConPos);
	Engine::Safe_Delete_Array(m_pVtxPos);

	CoUninitialize();
}