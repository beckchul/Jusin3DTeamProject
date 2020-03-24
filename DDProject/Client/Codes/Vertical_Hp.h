#pragma once

#include "OrthographicUI.h"

class CPlayerObserver;
class CVertical_Hp : public COrthographicUI
{
public:
	explicit CVertical_Hp(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CVertical_Hp();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	float			m_fTime = 0.f;
	CPlayerObserver*	m_pPlayerObserver = nullptr;
public:
	static CVertical_Hp* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

