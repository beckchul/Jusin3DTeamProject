#pragma once

#include "Font_Number.h"

class CWaveObserver;
class CFont_NextEnemies : public CFont_Number
{
public:
	explicit CFont_NextEnemies(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont_NextEnemies();
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
	static CFont_NextEnemies* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

