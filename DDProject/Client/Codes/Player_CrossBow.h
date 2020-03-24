#pragma once

#include "Defines.h"
#include "Weapon.h"

class CPlayer_CrossBow : public CWeapon
{
private:
	explicit CPlayer_CrossBow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer_CrossBow(void);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Create_Sphere(void) { return NOERROR; }
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CPlayer_CrossBow* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};
