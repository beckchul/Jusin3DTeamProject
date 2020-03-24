#ifndef Cube_Texture_h__
#define Cube_Texture_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCube_Texture : public CVIBuffer
{
private:
	explicit CCube_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCube_Texture(const CCube_Texture& rhs);
	virtual ~CCube_Texture(void);
public:
	HRESULT Ready_Buffer(void);
public:
	virtual void Render_Buffer(void);
public:
	virtual CComponent* Clone(void);
	static CCube_Texture* Create(LPDIRECT3DDEVICE9 pGraphicDev);	
	virtual void Free(void);
};


END

#endif // Cube_Texture_h__
