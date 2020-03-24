#pragma once

#include "Defines.h"
#include "Weapon.h"

class CPlayer_Staff : public CWeapon
{
private:
	explicit CPlayer_Staff(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer_Staff(void);

public:
	virtual void AttackMissile(_vec3 vDir, _int iAttPower);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	
private:
	virtual HRESULT Create_Sphere(void) { return NOERROR; }
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);


public:
	static CPlayer_Staff* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

