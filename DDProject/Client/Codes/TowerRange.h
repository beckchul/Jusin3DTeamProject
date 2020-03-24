#pragma once

#include "Font_Perspective.h"

class CTowerRange : public CFont_Perspective
{
public:
	explicit CTowerRange(LPDIRECT3DDEVICE9 pGraphicDev, int iTextureNum);
	virtual ~CTowerRange();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
	void Change_Color(_vec4 vecColor) { m_vecColor = vecColor; }
	_vec4 Get_Color(void) { return m_vecColor; }
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	_vec4					m_vecColor;
	_int					m_iTextureNum = 0;
public:
	static CTowerRange* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iTextureNum);
private:
	virtual void Free(void);
};

