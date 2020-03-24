#include "Collision.h"

USING(Engine)

CCollision::CCollision(LPDIRECT3DDEVICE9 pGraphicDev)
: m_pGraphicDev(pGraphicDev) 
{
	m_pGraphicDev->AddRef();
}

CCollision::CCollision(const CCollision & rhs)
: m_pGraphicDev(rhs.m_pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CCollision::~CCollision()
{
}

void CCollision::Free(void)
{
	Safe_Release(m_pGraphicDev);
}
