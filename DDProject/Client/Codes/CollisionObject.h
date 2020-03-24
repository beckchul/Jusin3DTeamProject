#pragma once

#include "Defines.h"
#include "StaticObject.h"

namespace Engine
{
	class CCollisionSphere;
}

class CCollisionManager;
class CCollisionObject : public CStaticObject
{
public:
	explicit CCollisionObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCollisionObject(void);
public:
	void Set_Damage(const _int& iAttackPower){
		m_tInfo.iAttPower = iAttackPower; }
protected:
	Engine::CCollisionSphere*			m_pCollisionSphere = nullptr;
	CCollisionManager*					m_pCollisionManager = nullptr;
	INFO								m_tInfo;
	_vec3								m_vDestPosition;	
	_matrix*							m_pEffectMatrix;
		
protected:
	virtual void Free(void);
};

