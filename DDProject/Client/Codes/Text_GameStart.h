#pragma once

#include "OrthographicUI.h"

class CText_GameStart : public COrthographicUI
{
public:
	explicit CText_GameStart(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CText_GameStart();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	_float					m_fTime = 20.f;
public:
	static CText_GameStart* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

