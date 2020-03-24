#ifndef Particle_Texture_h__
#define Particle_Texture_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CParticle_Texture : public CVIBuffer
{
private:
	explicit CParticle_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CParticle_Texture(const CParticle_Texture& rhs);
	virtual ~CParticle_Texture(void);
public:
	HRESULT Ready_Buffer(void);
public:
	virtual void Render_Buffer(void);

public:
	static CParticle_Texture* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);

private:
	VTXPARTICLE*		m_pVertex;

private:
	virtual void Free(void);
};


END

#endif // Particle_Texture_h__
