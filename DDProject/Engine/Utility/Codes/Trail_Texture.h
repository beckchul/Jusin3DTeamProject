#ifndef Trail_Texture_h__
#define Trail_Texture_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTrail_Texture : public CVIBuffer
{
private:
	explicit CTrail_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTrail_Texture(const CTrail_Texture& rhs);
	virtual ~CTrail_Texture(void);
public:
	HRESULT Ready_Buffer(void);
public:
	virtual void Render_Buffer(void);
public:
	void SetVertexTrail(const _vec3* pPos, const DWORD& dwCnt);
public:
	static CTrail_Texture* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
private:
	VTXTRAIL*		m_pVertex;
private:
	virtual void Free(void);
};


END

#endif // Trail_Texture_h__
