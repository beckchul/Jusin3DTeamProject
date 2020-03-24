#ifndef TextureRenderer_h__
#define TextureRenderer_h__

#include <streams.h>
#include "Engine_Defines.h"

BEGIN(Engine)

struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

class ENGINE_DLL CTextureRenderer : public CBaseVideoRenderer
{
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;

private:
	LONG m_lVidWidth;   // Video width
	LONG m_lVidHeight;  // Video Height
	LONG m_lVidPitch;   // Video Pitch

private:
	LPDIRECT3DTEXTURE9 m_pTexture;
	//D3DFORMAT m_TextureFormat;

public:
	CTextureRenderer(LPDIRECT3DDEVICE9 pGraphicDev, LPUNKNOWN pUnk, HRESULT* phr);
	virtual ~CTextureRenderer();

public:
	HRESULT CheckMediaType(const CMediaType *pmt );     // Format acceptable?
	HRESULT SetMediaType(const CMediaType *pmt );       // Video format notification
	HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

public:
	void SetTexture(DWORD dwStage);

public:
	void ReleaseTexture(void);
};

END

#endif // !TextureRenderer_h__
