#pragma once

#include "OrthographicUI.h"

class CWaveObserver;
class CFont_KillEnemies : public COrthographicUI
{
public:
	explicit CFont_KillEnemies(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont_KillEnemies();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	CWaveObserver*		m_pWaveObserver = nullptr;
	WAVE_INFO			m_tWaveInfo;
public:
	static CFont_KillEnemies* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

