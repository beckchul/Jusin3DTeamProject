#pragma once

#include "Font_Perspective.h"

class CTowerRange2 : public CFont_Perspective
{
public:
	explicit CTowerRange2(LPDIRECT3DDEVICE9 pGraphicDev, int iTextureNum);
	virtual ~CTowerRange2();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	void Change_Color(_vec4 vecColor) { m_vecColor = vecColor; }
private:
	_vec4					m_vecColor;
	_int					m_iTextureNum = 0;
public:
	static CTowerRange2* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iTextureNum);
private:
	virtual void Free(void);
};

