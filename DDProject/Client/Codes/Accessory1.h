#pragma once

#include "Defines.h"
#include "StaticObject.h"

class CAccessory1 : public CStaticObject
{
private:
	explicit CAccessory1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAccessory1(void);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CAccessory1* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

