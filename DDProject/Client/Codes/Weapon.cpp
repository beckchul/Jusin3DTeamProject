#include "stdafx.h"
#include "Weapon.h"
#include "InfoSubject.h"
#include "PlayerObserver.h"
#include "CollisionManager.h"

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
: CStaticObject(pGraphicDev)
, m_pPlayerMatirx(nullptr)
, m_pPlayerWeaponMatrix(nullptr)
, m_pCollisionSphere(nullptr)
, m_pCollisionManager(CCollisionManager::GetInstance())
{
	m_pCollisionManager->Add_Ref();
	m_pEffectPoint = new _matrix;
	D3DXMatrixIdentity(m_pEffectPoint);
}

CWeapon::~CWeapon()
{
}

void CWeapon::Free(void)
{
	Engine::Safe_Delete(m_pEffectPoint);
	Engine::Safe_Release(m_pCollisionSphere);
	Engine::Safe_Release(m_pCollisionManager);
	CStaticObject::Free();
}
