#pragma once

#include "OrthographicUI.h"

class CWaveObserver;
class CStaticUI : public COrthographicUI
{
public:
	explicit CStaticUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStaticUI();
public:
	virtual HRESULT Add_Component() { return NOERROR; }
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
private:
	list<COrthographicUI*>				m_listTextUI;
protected:
	CWaveObserver*		m_pWaveObserver = nullptr;
public:
	static CStaticUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

