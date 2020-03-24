#pragma once

#include "Defines.h"
#include "DynamicObject.h"

namespace Engine 
{
	class CCollisionSphere;
	class CNavMgr;
	class CInfoSubject;
}

class CPlayerObserver;
class CCollisionManager;
class CWaveObserver;
class CMonster : public CDynamicObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster(void);
protected:
	CWaveObserver*				m_pWaveObserver = nullptr;
	CInfoSubject*				m_pInfoSubject = nullptr;
	CCollisionManager*			m_pCollisionManager = nullptr;
protected:
	Engine::CCollisionSphere*	m_pCollisionBody    = nullptr;			// 충돌 몸체
	Engine::CCollisionSphere*	m_pCollsionWeapon   = nullptr;			// 충돌 무기
	_bool						m_bFirstFrame	    = false; 
protected:
	CPlayerObserver*			m_pPlayerObserver   = nullptr;			// 플레이어의 옵저버
	Engine::OBJECTID			m_eRouteID = OBJECT_END;
	vector<_vec3>				m_vecMonsterRoute;						// 몬스터 길
protected:
	FOLLOW_TARGERT				m_tFllowTarget;							// 플레이어 추적을 위한 구조체
	MONSTER_MAIN_STATE			m_eMainState		= MONSTER_IDLE;		// 몬스터의 메인 상태값
	MONSTER_HURT_STATE			m_eHurtState		= MONSTER_NORMAL;	// 몬스터의 상태이상 상태값
protected:
	MONSTER_TYPE				m_eMonsterType = MONSTER_TYPE_END;
	_int						m_iRenderState		= 0;
	_int						m_iPreRenderState	= 0;
	_int						m_iAttackState		= 0;				
	_float						m_fHitTime			= 0.f;				// 몬스터 피격 딜레이
	_float						m_fSlowSpeed		= 1.f;				// 몬스터의 속도
	_float						m_fLowDamage		= 1.f;				// 몬스터의 데미지 감소치
	_bool						m_bHurt				= true;
	_bool						m_bFuse				= false;
	_float						m_fDeadTrackPosition = 0.f;				//몬스터 사망 Frame
	_int						m_iDeadTrackIndex = 0;					//몬스터 사망 Index
	_float						m_fDeadTime = 0.f;						//몬스터 사망 후 Time
	_vec3						m_vDeadDir;
	_vec4						m_vecColor; // 컬러 조절하기위함.
	_vec4						m_vecTransColor; // 컬러조절하기위함.
	bool						m_bColorAdd = false; // 컬러 조절하기위함.
	float						m_fColorTime = 0.f; // 컬러 조절하기위함.
	float						m_fColorLimitTime = 0.f; //컬러 조절하기위함.
protected:
	_bool							m_bFly = false;
	_bool							m_bPreFly = false;
protected:
	// NavMesh
	DWORD						m_dwIndex			= -1;
	CNavMgr*			m_pNavMgr = nullptr;
protected:
	void Monster_Fall(const float& fTimeDelta); // 중력
	void Create_Route(Engine::OBJECTID eRoute);
protected:
	_bool Search_Target(const float& fTimeDelta);
	void Follow_Player(const float& fTimeDelta);
	void Work_Route(const float& fTimeDelta);
protected:
	void Play_Animation(const float& fTimeDelta);
	void Check_Collision(const float& fTimeDelta);
	virtual void Set_Collision(void)PURE;
protected:
	void Give_ManaToken(void);

protected:
	virtual void Free(void);
};

