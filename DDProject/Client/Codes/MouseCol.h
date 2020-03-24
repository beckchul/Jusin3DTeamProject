
#ifndef MouseCol_h__
#define MouseCol_h__

#include "Collision.h"
#include "Defines.h"

namespace Engine
{
	class CDataManager;
}

class CMouseCol : public Engine::CCollision
{
private:
	explicit CMouseCol(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMouseCol(void);
private:
	HRESULT	Initialize(LPD3DXMESH pMesh, _matrix& matWorld);
public:
	bool PickTerrain(_vec3* vecPos, _vec3* vecDir, _vec3* pOut); // Mesh로 된 지형들 피킹
public:
	virtual CComponent* Clone(void);
private:
	LPD3DXMESH				m_pCloneMesh = nullptr;
	Engine::VTXPOS*			m_pVertex;
	Engine::INDEX16*		m_pIndex;

	int						m_iFaceNum;
	CDataManager*			m_pDataMgr = nullptr;

public:
	static CMouseCol* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXMESH pMesh, _matrix& matWorld);
private:
	virtual void Free(void);
};

#endif // MouseCol_h__