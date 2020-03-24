#pragma once

#include "OrthographicUI.h"

class CWaveObserver;
class CFont_BossHP : public COrthographicUI
{
public:
	explicit CFont_BossHP(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont_BossHP();
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
	_int				m_iBossHp = 0;
public:
	static CFont_BossHP* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

