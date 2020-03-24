#pragma once

#include "OrthographicUI.h"

class CPlayerObserver;
class CPanel_ExpBar : public COrthographicUI
{
public:
	explicit CPanel_ExpBar(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPanel_ExpBar();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	//float a = 0.f, b = 0.f, c = 0.f;
	float				m_fTime = 0.f;
	float				m_fRatio = 0.f;
	CPlayerObserver*	m_pPlayerObserver = nullptr;
	Engine::CTexture*	m_pColorTexture = nullptr;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CPanel_ExpBar* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

