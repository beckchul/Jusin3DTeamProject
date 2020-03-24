#pragma once

#include "OrthographicUI.h"

class CPlayerObserver;
class CFont_Exp : public COrthographicUI
{
public:
	explicit CFont_Exp(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont_Exp();
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
	static CFont_Exp* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

