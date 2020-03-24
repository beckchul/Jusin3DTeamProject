#pragma once

#include "Defines.h"
#include "StaticObject.h"

class COldMap : public CStaticObject
{
private:
	explicit COldMap(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~COldMap(void);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static COldMap* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

