#include "stdafx.h"
#include "CollisionObject.h"
#include "CollisionManager.h"


CCollisionObject::CCollisionObject(LPDIRECT3DDEVICE9 pGraphicDev)
: CStaticObject(pGraphicDev)
, m_pCollisionManager(CCollisionManager::GetInstance())
, m_pCollisionSphere(nullptr)
{
	m_pCollisionManager->Add_Ref();

	ZeroMemory(&m_tInfo, sizeof(INFO));
	m_tInfo.bDead = false;
	//m_pGraphicDev->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS)
}

CCollisionObject::~CCollisionObject()
{
}

void CCollisionObject::Free(void)
{
	Engine::Safe_Release(m_pCollisionSphere);
	Engine::Safe_Release(m_pCollisionManager);
	CStaticObject::Free();
}
