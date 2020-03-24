#pragma once

#include "Monster.h"

class CMonster_DarkElfArcher : public CMonster
{
public:
	explicit CMonster_DarkElfArcher(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster_DarkElfArcher();
public:
	virtual HRESULT Ready_GameObject(_vec3& vPosition, Engine::OBJECTID eRoute);
	virtual int FirstUpdate_GameObject(const float& fTimeDelta);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void FirstRender_GameObject(_bool bTrue);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	void Change_State(void);
	virtual void Set_Collision(void);
public:
	static CMonster_DarkElfArcher* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3& vPosition, Engine::OBJECTID eRoute);
private:
	virtual void Free(void);
};

