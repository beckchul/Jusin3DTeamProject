#pragma once

#include "OrthographicUI.h"

class CUI_Check : public COrthographicUI
{
public:
	explicit CUI_Check(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI_Check();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CUI_Check* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};
