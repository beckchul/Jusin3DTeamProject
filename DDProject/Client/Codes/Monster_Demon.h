#pragma once

#include "Monster.h"

namespace Engine {	// Minkyeong
	class CEffect;
}

class CMonster_Demon : public CMonster
{
public:
	explicit CMonster_Demon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster_Demon();
public:
	virtual HRESULT Ready_GameObject(_vec3& vPosition);
	virtual int FirstUpdate_GameObject(const float& fTimeDelta);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void FirstRender_GameObject(_bool bTrue);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	_int			m_iPattonCount = 0;
	_float			m_fCount		= 0.f;

	list<Engine::CEffect*> m_listLHand;
	list<Engine::CEffect*> m_listRHand;

private:
	void Play_MonsterPatton(const float& fTimeDelta);
	void Search_Player(const float& fTimeDelta);
	void Change_State(void);
	virtual void Set_Collision(void);
public:
	static CMonster_Demon* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3& vPosition);
private:
	virtual void Free(void);
};

