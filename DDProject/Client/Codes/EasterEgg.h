#pragma once

#include "Defines.h"
#include "StaticObject.h"

class CEasterEgg : public CStaticObject
{
private:
	explicit CEasterEgg(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEasterEgg(void);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CEasterEgg* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

