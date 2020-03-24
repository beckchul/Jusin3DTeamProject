#pragma once

#include "OrthographicUI.h"

class CMiniMap : public COrthographicUI
{
public:
	explicit CMiniMap(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMiniMap();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	
private:
	bool			m_bShowMiniMap;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CMiniMap* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

