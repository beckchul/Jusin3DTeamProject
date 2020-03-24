#ifndef SplashMissile_h__
#define SplashMissile_h__

#include "Defines.h"
#include "CollisionObject.h"

namespace Engine
{
	class CEffect;
}

class CSplashMissile : public CCollisionObject
{
private:
	explicit CSplashMissile(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSplashMissile(void);
public:
	virtual HRESULT Ready_GameObject(_vec3 vPosition, _vec3 _vDir, SPLASH_TYPE eType);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	_vec3 m_vDir;
	list<Engine::CEffect*>	m_Templist;
	SPLASH_TYPE m_eType;
private:
	virtual void Check_Collision(void);
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CSplashMissile* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, _vec3 vDir, SPLASH_TYPE eType);
protected:
	virtual void Free(void);
};

#endif