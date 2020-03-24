#pragma once

#include "Defines.h"
#include "StaticObject.h"

namespace Engine
{
	class CCollisionSphere;
}

class CCollisionManager;
class CCrystal : public CStaticObject
{
private:
	explicit CCrystal(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCrystal(void);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	Engine::CCollisionSphere*			m_pCollisionSphere = nullptr;
	CCollisionManager*					m_pCollisionManager = nullptr;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CCrystal* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

