#pragma once

#include "OrthographicUI.h"

class CPlayerObserver;
class CPanel_newBar : public COrthographicUI
{
public:
	explicit CPanel_newBar(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPanel_newBar();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	bool				m_bShowPanel_newBar;
	float				m_fTime = 0.f;
	CPlayerObserver*	m_pPlayerObserver = nullptr;
	Engine::CTexture*	m_pMaskTexture = nullptr;
	Engine::CTexture*	m_pColorTexture = nullptr;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CPanel_newBar* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

