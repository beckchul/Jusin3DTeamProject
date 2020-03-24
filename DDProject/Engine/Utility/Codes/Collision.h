#ifndef Collision_h__
#define Collision_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollision : public CComponent
{
protected:
	explicit CCollision(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollision(const CCollision& rhs);
	virtual ~CCollision(void);

public:
	virtual CComponent* Clone(void) override PURE;
protected:
	LPDIRECT3DDEVICE9		m_pGraphicDev;

protected:
	virtual void Free(void) override;
};

END

#endif