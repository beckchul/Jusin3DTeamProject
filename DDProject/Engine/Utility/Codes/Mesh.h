#ifndef Mesh_h__
#define Mesh_h__

#include "Resources.h"

BEGIN(Engine)

class ENGINE_DLL CMesh : public CResources
{
public:
	enum MESHTYPE {TYPE_STATIC, TYPE_DYNAMIC, TYPE_NAVIGATION};
protected:
	explicit CMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh(void);
protected:
	_bool					m_isClone;
public:
	virtual CComponent* Clone(void)PURE;
public:
	virtual void Free(void);

};


END

#endif // Mesh_h__
