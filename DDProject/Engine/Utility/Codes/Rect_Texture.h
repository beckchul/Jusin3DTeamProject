#ifndef Rect_Texture_h__
#define Rect_Texture_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRect_Texture : public CVIBuffer
{
private:
	explicit CRect_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRect_Texture(const CRect_Texture& rhs);
	virtual ~CRect_Texture(void);
public:
	HRESULT Ready_Buffer(void);
public:
	virtual void Render_Buffer(void);
public:
	static CRect_Texture* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
private:
	virtual void Free(void);
};


END

#endif // Rect_Texture_h__
