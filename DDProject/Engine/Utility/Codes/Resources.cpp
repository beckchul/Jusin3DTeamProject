#include "Resources.h"

USING(Engine)

CResources::CResources(LPDIRECT3DDEVICE9 pGraphicDev)
: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
	
}

CResources::CResources(const CResources& rhs)
: m_pGraphicDev(rhs.m_pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CResources::~CResources(void)
{

}

void CResources::Free(void)
{
	Engine::Safe_Release(m_pGraphicDev);
}
