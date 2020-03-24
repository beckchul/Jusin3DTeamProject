#pragma once

#include "OrthographicUI.h"

class CLastManObserver;
class CPlayer_Left : public COrthographicUI
{
public:
	explicit CPlayer_Left(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer_Left();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	CLastManObserver*			m_pLastManObserver = nullptr;
public:
	static CPlayer_Left* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

