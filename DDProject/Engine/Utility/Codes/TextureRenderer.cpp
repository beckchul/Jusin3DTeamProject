#include "TextureRenderer.h"

Engine::CTextureRenderer::CTextureRenderer(LPDIRECT3DDEVICE9 pGraphicDev, LPUNKNOWN pUnk, HRESULT* phr)
:CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer), NAME("Texture Renderer"), pUnk, phr)
,m_lVidWidth(0)
,m_lVidHeight(0)
,m_lVidPitch(0)
,m_pTexture(NULL)
, m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();

	// Store and AddRef the texture for our use.
	ASSERT(phr);
	if (phr)
		*phr = S_OK;
}

Engine::CTextureRenderer::~CTextureRenderer()
{
	ReleaseTexture();
}

HRESULT Engine::CTextureRenderer::CheckMediaType(const CMediaType *pmt )
{
	HRESULT   hr = E_FAIL;
	VIDEOINFO *pvi=0;

	CheckPointer(pmt,E_POINTER);

	// Reject the connection if this is not a video type
	if( *pmt->FormatType() != FORMAT_VideoInfo ) {
		return E_INVALIDARG;
	}

	// Only accept RGB24 video
	pvi = (VIDEOINFO *)pmt->Format();

	if(IsEqualGUID( *pmt->Type(),    MEDIATYPE_Video)  &&
		IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24))
	{
		hr = S_OK;
	}

	return hr;
}

HRESULT Engine::CTextureRenderer::SetMediaType(const CMediaType *pmt )
{
	HRESULT hr;

	VIDEOINFO *pviBmp;                      // Bitmap info header
	pviBmp = (VIDEOINFO *)pmt->Format();

	m_lVidWidth  = pviBmp->bmiHeader.biWidth;
	m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
	m_lVidPitch  = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24

	// Create the texture that maps to this media type
	hr =m_pGraphicDev->CreateTexture(m_lVidWidth , m_lVidHeight , 1, 0,
		D3DFMT_X8R8G8B8 , D3DPOOL_MANAGED,
		&m_pTexture , NULL);
	if( FAILED(hr))
	{
		return hr;
	}
	return S_OK;
}

HRESULT Engine::CTextureRenderer::DoRenderSample(IMediaSample *pMediaSample)
{
	BYTE  *pBmpBuffer , *pTxtBuffer; // Bitmap buffer , texture buffer
	LONG  lTxtPitch;                // Pitch of bitmap , texture

	BYTE  * pbS = NULL;
	DWORD * pdwS = NULL;
	DWORD * pdwD = NULL;

	CheckPointer(pMediaSample , E_POINTER);
	CheckPointer(m_pTexture , E_UNEXPECTED);

	// Get the video bitmap buffer
	pMediaSample->GetPointer( &pBmpBuffer );

	// Lock the Texture
	D3DLOCKED_RECT d3dlr;

	if (FAILED(m_pTexture->LockRect(0, &d3dlr , 0, 0)))
		return E_FAIL;

	// Get the texture buffer & pitch
	pTxtBuffer = static_cast<byte *>(d3dlr.pBits);
	lTxtPitch = d3dlr.Pitch;

	// Copy the bits    
	pTxtBuffer += (m_lVidHeight-1) * lTxtPitch;
	for(int y=0; y<m_lVidHeight; y++ )
	{
		BYTE *pBmpBufferOld = pBmpBuffer;
		BYTE *pTxtBufferOld = pTxtBuffer;   
		for (int x = 0; x < m_lVidWidth; x++)
		{
			pTxtBuffer[0] = pBmpBuffer[0];
			pTxtBuffer[1] = pBmpBuffer[1];
			pTxtBuffer[2] = pBmpBuffer[2];
			pTxtBuffer[3] = 0xff;
			pTxtBuffer += 4;
			pBmpBuffer += 3;
		}
		pBmpBuffer = pBmpBufferOld + m_lVidPitch;
		pTxtBuffer = pTxtBufferOld - lTxtPitch;
	}

	// Unlock the Texture
	if (FAILED(m_pTexture->UnlockRect(0)))
		return E_FAIL;

	return S_OK;
}

void Engine::CTextureRenderer::ReleaseTexture(void)
{	
	Engine::Safe_Release(m_pGraphicDev);
	Engine::Safe_Release(m_pTexture);
}

void Engine::CTextureRenderer::SetTexture(DWORD dwStage)
{
	if(m_pTexture != NULL)
		m_pGraphicDev->SetTexture(dwStage, m_pTexture);
}	