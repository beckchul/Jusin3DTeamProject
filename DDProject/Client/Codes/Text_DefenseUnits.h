#pragma once

#include "OrthographicUI.h"

class CText_DefenseUnits : public COrthographicUI
{
public:
	explicit CText_DefenseUnits(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CText_DefenseUnits();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CText_DefenseUnits* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

