#pragma once

#include "OrthographicUI.h"

class CPanel_Wave : public COrthographicUI
{
public:
	explicit CPanel_Wave(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPanel_Wave();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	bool			m_bShowPanel_Wave;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CPanel_Wave* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

