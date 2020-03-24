#pragma once

#include "Defines.h"
#include "CollisionObject.h"

class CCrossBow_Arrow : public CCollisionObject
{
private:
	explicit CCrossBow_Arrow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCrossBow_Arrow(void);
public:
	virtual HRESULT Ready_GameObject(_vec3 vPosition, _vec3 _vDir, _vec3 vAngle);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0); 
private:
		_vec3 m_vDir;
private:
	virtual void Check_Collision(void);
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CCrossBow_Arrow* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, _vec3 _vDir, _vec3 vAngle);
protected:
	virtual void Free(void);
};