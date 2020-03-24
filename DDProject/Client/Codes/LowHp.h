#pragma once

#include "OrthographicUI.h"

class CLowHp : public COrthographicUI
{
public:
	explicit CLowHp(LPDIRECT3DDEVICE9 pGraphicDev, _float fExistTime);
	virtual ~CLowHp();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	_float			m_fTime = 0.f;
public:
	static CLowHp* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fExistTime);
private:
	virtual void Free(void);
};

