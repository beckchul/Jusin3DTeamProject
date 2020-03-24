#pragma once

#include "Defines.h"
#include "Weapon.h"

class CPlayer_Spear : public CWeapon
{
private:
	explicit CPlayer_Spear(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer_Spear(void);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Create_Sphere(void);
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CPlayer_Spear* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

