#pragma once

#include "Defines.h"
#include "StaticObject.h"

namespace Engine
{ 
	class CCollisionSphere;
}
class CPlayerObserver;
class CCollisionManager;
class CWeapon : public CStaticObject
{
protected:
	explicit CWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWeapon(void);
public:
	void Set_CollisionCheck(_bool bCheck) {
 		if(nullptr != m_pCollisionSphere)
		m_pCollisionSphere->Set_CheckCollsion(bCheck); }
	void Set_MatrixInfo(_matrix* pPlayerMat, _matrix* pPlayerWeaponMat)
	{
		m_pPlayerMatirx = pPlayerMat;
		m_pPlayerWeaponMatrix = pPlayerWeaponMat;
	}
	void Set_Damage(_int iDamage) {
		if (nullptr != m_pCollisionSphere)
			m_pCollisionSphere->Set_Damage(iDamage); }
public:
	_matrix* Get_EffectPointMatrix(void) {
		return m_pEffectPoint; }

public:
	virtual void AttackMissile(_vec3 vDir, _int iAttPower) {}
public:
	virtual HRESULT Create_Sphere(void)PURE;
protected:
	const _matrix*					m_pPlayerMatirx = nullptr;
	const _matrix*					m_pPlayerWeaponMatrix = nullptr;
	_matrix*							m_pEffectPoint = nullptr;
protected:
	Engine::CCollisionSphere*			m_pCollisionSphere = nullptr;
	CCollisionManager*					m_pCollisionManager = nullptr;
protected:
	virtual void Free(void);
};

