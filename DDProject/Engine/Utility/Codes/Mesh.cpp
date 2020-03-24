#include "Mesh.h"

USING(Engine)

Engine::CMesh::CMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	:CResources(pGraphicDev)
	, m_isClone(false)
{
	
}

CMesh::CMesh(const CMesh& rhs)
	: CResources(rhs)
	, m_isClone(true)	
{
	
}

Engine::CMesh::~CMesh(void)
{

}


void Engine::CMesh::Free(void)
{
	CResources::Free();
}

