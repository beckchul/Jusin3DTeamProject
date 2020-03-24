#pragma once

#include "OrthographicUI.h"

class CText_ToLobby : public COrthographicUI
{
public:
	explicit CText_ToLobby(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CText_ToLobby();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	_float					m_fTime = 5.f;
public:
	static CText_ToLobby* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

