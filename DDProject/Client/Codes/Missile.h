#ifndef Missile_h__
#define Missile_h__

#include "Defines.h"
#include "CollisionObject.h"

namespace Engine {	// Minkyeong
	class CEffect;
}

class CMissile : public CCollisionObject
{
private:
	explicit CMissile(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMissile(void);
public:
	virtual HRESULT Ready_GameObject(_vec3 vPosition, _vec3 _vDir, MISSILE_TYPE eType);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	_vec3					m_vDir;
	MISSILE_TYPE		m_eMissileType = MISSILE_TYPE_END;

private:		// Minkyeong
	list<Engine::CEffect*>	m_Templist;

private:
	virtual void Check_Collision(void);
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CMissile* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, _vec3 vDir, MISSILE_TYPE eType);
protected:
	virtual void Free(void);
};

#endif