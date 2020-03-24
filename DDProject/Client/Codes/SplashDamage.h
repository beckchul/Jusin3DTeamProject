#ifndef SplashDamage_h__
#define SplashDamage_h__

#include "Defines.h"
#include "CollisionObject.h"

namespace Engine
{
	class CEffect;
}

class CSplashDamage : public CCollisionObject
{
private:
	explicit CSplashDamage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSplashDamage(void);
public:
	virtual HRESULT Ready_GameObject(_vec3 vPosition, SPLASH_TYPE eType);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	_matrix*		m_pEffectMatrix;
	SPLASH_TYPE		m_eType;
	_float			m_fTimeCount = 0.f;
	_float			m_fLifeTime = 0.f;
	_bool			m_bCheck = false;
	
private:
	virtual void Check_Collision(void);
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CSplashDamage* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, SPLASH_TYPE eType);
protected:
	virtual void Free(void);
};

#endif