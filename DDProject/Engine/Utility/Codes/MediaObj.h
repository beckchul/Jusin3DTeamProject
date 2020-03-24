#ifndef MediaObj_h__
#define MediaObj_h__

#include "TextureRenderer.h"
#include "atlbase.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CTextureRenderer;
class ENGINE_DLL CMediaObj : public CBase
{
public:
	CMediaObj(void);
	~CMediaObj(void);

private:
	CComPtr<IGraphBuilder>  m_pGB;          // GraphBuilder
	CComPtr<IMediaControl>  m_pMC;          // Media Control
	CComPtr<IMediaPosition> m_pMP;          // Media Position
	CComPtr<IMediaEvent>    m_pME;          // Media Event
	CComPtr<IBaseFilter>    m_pRenderer;    // our custom renderer
	CTextureRenderer*		pCTR;			// TextureRenderer 

private:
	BOOL m_bPause;

public:
	HRESULT Initialize(LPDIRECT3DDEVICE9 pGraphicDev, TCHAR* wFileName);

public:
	void SetTexMovie(DWORD dwStage);
	void SetTime(double time) {m_pMP->put_CurrentPosition(time);}

public:
	double GetStopTime(void);
	double GetCurrentPosition(void) const;

public:
	BOOL Play(void);

private:
	virtual void Free(void);
};

END

#endif