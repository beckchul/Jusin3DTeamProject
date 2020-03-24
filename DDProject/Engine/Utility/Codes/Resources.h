#ifndef Resources_h__
#define Resources_h__

#include "Component.h"
#include "GraphicDev.h"
BEGIN(Engine)
class ENGINE_DLL CResources : public CComponent
{
protected:
	explicit CResources(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CResources(const CResources& rhs);
	virtual ~CResources(void);

public:
	virtual CComponent* Clone(void) override PURE;
	
protected:
	LPDIRECT3DDEVICE9		m_pGraphicDev;

protected:
	virtual void Free(void) override;
};
END
#endif