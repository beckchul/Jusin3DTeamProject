#pragma once

#include "Font_Number.h"

class CFont_FPS : public CFont_Number
{
public:
	explicit CFont_FPS(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont_FPS();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CFont_FPS* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	bool				m_bShowFPS = false;
	int					m_iFps = 0;
	float				m_fTime = 0.f;
private:
	virtual void Free(void);
};

