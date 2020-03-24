#pragma once

#include "Defines.h"
#include "StaticObject.h"

class CMagusQuarters : public CStaticObject
{
public:
	explicit CMagusQuarters(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMagusQuarters();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	bool		m_bShowMap = true;
public:
	static CMagusQuarters* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

