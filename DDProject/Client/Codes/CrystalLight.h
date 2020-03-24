#pragma once

#include "Defines.h"
#include "StaticObject.h"

class CCrystalLight : public CStaticObject
{
//public:
	//enum CrystalLight_Color { CRYSTALLIGHT_BLUE, CRYSTALLIGHT_YELLOW, CRYSTALLIGHT_GREEN, CRYSTALLIGHT_PURPLE };
private:
	explicit CCrystalLight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCrystalLight(void);
public:
	void Set_Color(D3DXCOLOR& Color) { m_Color = Color; }
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
private:
	//CrystalLight_Color	m_eColor = CRYSTALLIGHT_BLUE;
	D3DXCOLOR m_Color = D3DXCOLOR(0.f, 0.84f, 1.f, 0.f);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CCrystalLight* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

