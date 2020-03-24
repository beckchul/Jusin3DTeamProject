#pragma once

#include "Defines.h"
#include "CollisionObject.h"

class CMonster_Arrow : public CCollisionObject
{
private:
	explicit CMonster_Arrow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster_Arrow(void);
public:
	virtual HRESULT Ready_GameObject(_vec3 vPosition, _vec3 vAngle);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual void Check_Collision(void);
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CMonster_Arrow* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, _vec3 vAngle);
protected:
	virtual void Free(void);
};