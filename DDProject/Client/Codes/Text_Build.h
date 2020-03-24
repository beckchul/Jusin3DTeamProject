#pragma once

#include "OrthographicUI.h"

class CWaveObserver;
class CText_Build : public COrthographicUI
{
public:
	explicit CText_Build(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CText_Build();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	CWaveObserver*		m_pWaveObserver = nullptr;
public:
	static CText_Build* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

