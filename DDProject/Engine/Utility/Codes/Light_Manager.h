#ifndef Light_Manager_h__
#define Light_Manager_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class CLight;
class ENGINE_DLL CLight_Manager : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	explicit CLight_Manager(void);
	virtual ~CLight_Manager(void);
public:
	HRESULT Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo);
	void Render_Light(LPD3DXEFFECT pEffect);
	void Reset_Light(void);
private:
	list<CLight*>				m_LightList;
	typedef list<CLight*>		LIGHTLIST;	
private:
	virtual void Free(void);
};

END

#endif // Light_Manager_h__
