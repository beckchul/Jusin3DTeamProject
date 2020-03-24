#pragma once

#include "Defines.h"
#include "StaticObject.h"

namespace Engine
{
	class CCollisionSphere;
	class CEffect;
}

class CCollisionManager;
class CAura : public CStaticObject
{
public:
	explicit CAura(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAura(void);
public:
	void SetColor(const _vec4& vecColor) { m_vecColor = vecColor; }
	void Set_Summon(bool b) { m_bSummoning = b; }
	
public:
	virtual HRESULT Ready_GameObject(_vec3 vPosition, TOWER_TYPE eType);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	Engine::CCollisionSphere*			m_pCollisionBody = nullptr;
	CCollisionManager*					m_pCollisionManager = nullptr;

	list<Engine::CEffect*>				m_list_EffectInsideAura;
	list<Engine::CEffect*>				m_list_EffectAura;
private:
	TOWER_TYPE							m_eTowerType = TOWER_TYPE_END;
	_float								m_fCount = 0.f;
	_float								m_fAttackDelay = 0.f;
	_float								m_fLifeTime = 0.f;

	CGameObject*						m_pUIObject = nullptr;
private:
	_vec4			m_vecColor;
	bool			m_bSummoning = false;
public:
	static CAura* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition,TOWER_TYPE eType);
private:
	virtual void Free(void);
};

