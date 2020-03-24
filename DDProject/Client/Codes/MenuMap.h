#pragma once

#include "Defines.h"
#include "StaticObject.h"

class CMenuMap : public CStaticObject
{
public:
	explicit CMenuMap(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMenuMap();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CMenuMap* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

