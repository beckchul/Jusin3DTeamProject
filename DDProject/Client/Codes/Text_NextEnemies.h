#pragma once

#include "OrthographicUI.h"

class CWaveObserver;
class CText_NextEnemies : public COrthographicUI
{
public:
	explicit CText_NextEnemies(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CText_NextEnemies();
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
	static CText_NextEnemies* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

