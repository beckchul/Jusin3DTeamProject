#pragma once

#include "OrthographicUI.h"

class CPanel_Level : public COrthographicUI
{
public:
	explicit CPanel_Level(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPanel_Level();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	bool			m_bShowPanel_Level;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CPanel_Level* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

