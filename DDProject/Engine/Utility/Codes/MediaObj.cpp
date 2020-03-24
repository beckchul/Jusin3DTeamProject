#include "MediaObj.h"



Engine::CMediaObj::CMediaObj(void)
{
}

Engine::CMediaObj::~CMediaObj(void)
{
}

HRESULT Engine::CMediaObj::Initialize(LPDIRECT3DDEVICE9 pGraphicDev, TCHAR* wFileName)
{
	HRESULT hr = S_OK;
	CComPtr<IBaseFilter>    pFSrc;          // Source Filter
	CComPtr<IPin>           pFSrcPinOut;    // Source Filter Output Pin   

	// Create the filter graph
	if (FAILED(m_pGB.CoCreateInstance(CLSID_FilterGraph , nullptr, CLSCTX_INPROC)))
		return E_FAIL;

	// Create the Texture Renderer object
	pCTR = new CTextureRenderer(pGraphicDev, nullptr, &hr);
	if (FAILED(hr) || !pCTR)
		return E_FAIL;

	// Get a pointer to the IBaseFilter on the TextureRenderer , add it to graph
	m_pRenderer = pCTR;
	if (FAILED(hr = m_pGB->AddFilter(m_pRenderer , L"TextureRenderer")))
		return hr;

	// Add the source filter to the graph.
	hr = m_pGB->AddSourceFilter(wFileName , L"SOURCE", &pFSrc);

	// If the media file was not found , inform the user.
	if (hr == VFW_E_NOT_FOUND)
		return hr;
	else if(FAILED(hr))
		return hr;

	if (SUCCEEDED(hr = pFSrc->FindPin(L"Output", &pFSrcPinOut)))
	{
		if (FAILED(hr = m_pGB->Render(pFSrcPinOut)))
			return hr;
	}

	// Get the graph's media control , event & position interfaces
	m_pGB.QueryInterface(&m_pMC);
	m_pGB.QueryInterface(&m_pMP);
	m_pGB.QueryInterface(&m_pME);

	// Start the graph running;
	if (FAILED(hr = m_pMC->Run()))
		return hr;

	return S_OK;
}


void Engine::CMediaObj::SetTexMovie(DWORD dwStage)
{
	pCTR->SetTexture(dwStage);
}

double Engine::CMediaObj::GetStopTime(void)
{
	REFTIME time;
	m_pMP->get_StopTime(&time);
	return time;
}

double Engine::CMediaObj::GetCurrentPosition() const
{
	REFTIME time;
	m_pMP->get_CurrentPosition(&time);
	return time;
}

BOOL Engine::CMediaObj::Play(void)
{
	if(m_bPause == TRUE)
	{
		m_bPause = FALSE;
		if(m_pMC != NULL)
			m_pMC->Run();
		else
			return FALSE;
	}

	return TRUE;
}

void Engine::CMediaObj::Free(void)
{
	// Shut down the graph
	if (!(!m_pMC)) m_pMC->Stop();

	if (!(!m_pMC)) m_pMC.Release();
	if (!(!m_pME)) m_pME.Release();
	if (!(!m_pMP)) m_pMP.Release();
	if (!(!m_pGB)) m_pGB.Release();
	if (!(!m_pRenderer)) m_pRenderer.Release();
}
