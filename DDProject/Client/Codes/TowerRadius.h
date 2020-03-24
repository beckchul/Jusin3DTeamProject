#pragma once

#include "Font_Perspective.h"

class CTowerRadius : public CFont_Perspective
{
public:
	explicit CTowerRadius(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTowerRadius();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	float			m_fAcc = 0.f;
public:
	static CTowerRadius* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

