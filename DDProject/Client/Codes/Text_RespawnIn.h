#pragma once

#include "OrthographicUI.h"

class CPlayerObserver;
class CText_RespawnIn : public COrthographicUI
{
public:
	explicit CText_RespawnIn(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CText_RespawnIn();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	CPlayerObserver*		m_pPlayerObserver = nullptr;
public:
	static CText_RespawnIn* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

