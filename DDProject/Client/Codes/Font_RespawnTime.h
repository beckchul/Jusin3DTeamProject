#pragma once

#include "OrthographicUI.h"

class CPlayerObserver;
class CFont_RespawnTime : public COrthographicUI
{
public:
	explicit CFont_RespawnTime(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont_RespawnTime();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	CPlayerObserver*		m_pPlayerObserver = nullptr;
	_float					m_fTime = 0.f;
public:
	static CFont_RespawnTime* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

