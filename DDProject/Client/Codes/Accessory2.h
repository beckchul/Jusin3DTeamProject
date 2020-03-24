#pragma once

#include "Defines.h"
#include "StaticObject.h"

class CAccessory2 : public CStaticObject
{
private:
	explicit CAccessory2(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAccessory2(void);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CAccessory2* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

