#pragma once

#include "Font_Number.h"

class CFont_Score : public CFont_Number
{
public:
	explicit CFont_Score(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont_Score();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CFont_Score* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

