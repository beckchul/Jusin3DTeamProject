#ifndef GraphicDev_h__
#define GraphicDev_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CGraphicDev : public CBase
{
	DECLARE_SINGLETON(CGraphicDev)

public:
	enum WINMODE {MODE_FULL, MODE_WIN};

private:
	explicit CGraphicDev(void);
	virtual ~CGraphicDev(void);

public: // getter
	LPDIRECT3DDEVICE9 Get_GraphicDev(void) const 
	{
		m_pGraphicDev->AddRef();
		return m_pGraphicDev;
	}

public:
	HRESULT Ready_GraphicDev(WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY);	

private:
	LPDIRECT3D9					m_pSDK = nullptr;
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;

private:
	void SetParameters(D3DPRESENT_PARAMETERS& d3dpp, WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY);

private:
	virtual void Free(void) override;
};

END

#endif // GraphicDev_h__
