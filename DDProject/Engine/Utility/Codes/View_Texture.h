#ifndef View_Texture_h__
#define View_Texture_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CView_Texture : public CVIBuffer
{
private:
	explicit CView_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CView_Texture(const CView_Texture& rhs);
	virtual ~CView_Texture(void);
public:
	HRESULT Ready_Buffer(_vec4* pBufferInfo);
public:
	virtual void Render_Buffer();
public:
	virtual CComponent* Clone(void);
	static CView_Texture* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec4* pBufferInfo);
	virtual void Free(void);
};

END

#endif // View_Texture_h__
