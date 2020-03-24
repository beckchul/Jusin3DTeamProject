#pragma once

#include "Defines.h"
#include "DynamicObject.h"
#include "DataManager.h"

namespace Engine
{
	class CCollisionSphere;
	class CEffect;
}

class CCollisionManager;
class CTower : public CDynamicObject
{
public:
	explicit CTower(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTower(void);
	// Setter
public:
	void SetColor(const _vec4& vecColor) { m_vecPrevColor = vecColor; }
	void Set_Summon(bool b) { m_bSummoning = b; }
	void Set_TowerEffect(TOWER_TYPE eType);	// 민경 : 타워가 실제적으로 생성시 이펙트(Under,Ball) 가지게함
public:
	void Appear(const float& fTimeDelta, const float& fScale); // 등장할 때 커지는거

protected:
	Engine::CCollisionSphere*			m_pTargetSphere = nullptr;
	Engine::CCollisionSphere*			m_pCollisionBody = nullptr;
	CCollisionManager*					m_pCollisionManager = nullptr;

protected:
	TOWER_STATE							m_eMainState = TOWER_END;
	_int								m_iRenderState = 0;
	_int								m_iPreRenderState = 0;
	_int								m_iAttackCount = 0;
	_float								m_fHitTime = 0.f;
	_float								m_fCount = 0.f;
	_float								m_fAttackDelay = 0.f;
	_float								m_fAttackDelayCount = 0.f;
	TARGET_CHECK						m_tTargetCheck;

	_vec4								m_vecColor; // 컬러 조절하기위함.
	_vec4								m_vecTransColor; // 컬러조절하기위함.
	bool								m_bColorAdd = false; // 컬러 조절하기위함.
	float								m_fColorTime = 0.f; // 컬러 조절하기위함.
	float								m_fColorLimitTime = 0.f; //컬러 조절하기위함.

protected:
	void Check_Collision(void);
	void Check_Target(void);
	void Check_TargetAll(void);
	void Play_Animation(const float& fTimeDelta);

protected:
	void Change_MainState(void);
	virtual void Change_State(void)PURE;
	virtual void Create_Bullet(void)PURE;
protected:
	_vec4			m_vecPrevColor;
	BOOL			m_bSummoning = false;
protected:
	list<CEffect*> m_pTower_BallEffect;
	list<CEffect*> m_pTower_UnderEffect;
protected:
	virtual void Free(void);
};

