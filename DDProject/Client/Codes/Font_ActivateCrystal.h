#pragma once

#include "Font_Perspective.h"

namespace Engine
{
	class CDataManager;
}

class CPlayerObserver;
class CFont_ActivateCrystal : public CFont_Perspective
{
public:
	explicit CFont_ActivateCrystal(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont_ActivateCrystal();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	CDataManager*					m_pDataMgr = nullptr;
	CPlayerObserver*				m_pPlayerObserver = nullptr;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CFont_ActivateCrystal* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

