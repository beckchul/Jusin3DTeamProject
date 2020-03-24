#pragma once

#include "OrthographicUI.h"

class CWaveObserver;
class CPanel_MonsterKill : public COrthographicUI
{
public:
	explicit CPanel_MonsterKill(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPanel_MonsterKill();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	bool				m_bShowPanel_MonsterKill;
	float				m_fRatio = 0.f;
	float				m_fTime = 0.f;
	CWaveObserver*		m_pWaveObserver = nullptr;
	Engine::CTexture*	m_pMaskTexture = nullptr;
	Engine::CTexture*	m_pColorTexture = nullptr;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CPanel_MonsterKill* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

