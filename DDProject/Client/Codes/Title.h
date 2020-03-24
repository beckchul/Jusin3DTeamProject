#pragma once

#include "Defines.h"
#include "StaticObject.h"

class CSceneObserver;
class CTitle : public CStaticObject
{
private:
	explicit CTitle(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTitle(void);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	CSceneObserver*		m_pSceneObserver = nullptr;
	float			m_fTime = 0.f;
public:
	static CTitle* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

