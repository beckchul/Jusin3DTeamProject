#ifndef Light_h__
#define Light_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class CView_Texture;
class ENGINE_DLL CLight : public CBase
{
private:
	explicit CLight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLight(void);
public:
	const D3DLIGHT9* GetLightInfo(void) { return &m_LightInfo; }

public:
	HRESULT Ready_Light(const D3DLIGHT9* pLightInfo);
	void Render_Light(LPD3DXEFFECT pEffect);
private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	CView_Texture*				m_pBufferCom = nullptr;
private:
	D3DLIGHT9					m_LightInfo;
public:
	static CLight* Create(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo);
private:
	virtual void Free(void);
};

END

#endif // Light_h__
