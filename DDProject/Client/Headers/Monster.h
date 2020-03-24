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
	Engine::CCollisionSphere*	m_pCollisionBody    = nullptr;			// �浹 ��ü
	Engine::CCollisionSphere*	m_pCollsionWeapon   = nullptr;			// �浹 ����
	_bool						m_bFirstFrame	    = false; 
protected:
	CPlayerObserver*			m_pPlayerObserver   = nullptr;			// �÷��̾��� ������
	Engine::OBJECTID			m_eRouteID = OBJECT_END;
	vector<_vec3>				m_vecMonsterRoute;						// ���� ��
protected:
	FOLLOW_TARGERT				m_tFllowTarget;							// �÷��̾� ������ ���� ����ü
	MONSTER_MAIN_STATE			m_eMainState		= MONSTER_IDLE;		// ������ ���� ���°�
	MONSTER_HURT_STATE			m_eHurtState		= MONSTER_NORMAL;	// ������ �����̻� ���°�
protected:
	MONSTER_TYPE				m_eMonsterType = MONSTER_TYPE_END;
	_int						m_iRenderState		= 0;
	_int						m_iPreRenderState	= 0;
	_int						m_iAttackState		= 0;				
	_float						m_fHitTime			= 0.f;				// ���� �ǰ� ������
	_float						m_fSlowSpeed		= 1.f;				// ������ �ӵ�
	_float						m_fLowDamage		= 1.f;				// ������ ������ ����ġ
	_bool						m_bHurt				= true;
	_bool						m_bFuse				= false;
	_float						m_fDeadTrackPosition = 0.f;				//���� ��� Frame
	_int						m_iDeadTrackIndex = 0;					//���� ��� Index
	_float						m_fDeadTime = 0.f;						//���� ��� �� Time
	_vec3						m_vDeadDir;
	_vec4						m_vecColor; // �÷� �����ϱ�����.
	_vec4						m_vecTransColor; // �÷������ϱ�����.
	bool						m_bColorAdd = false; // �÷� �����ϱ�����.
	float						m_fColorTime = 0.f; // �÷� �����ϱ�����.
	float						m_fColorLimitTime = 0.f; //�÷� �����ϱ�����.
protected:
	_bool							m_bFly = false;
	_bool							m_bPreFly = false;
protected:
	// NavMesh
	DWORD						m_dwIndex			= -1;
	CNavMgr*			m_pNavMgr = nullptr;
protected:
	void Monster_Fall(const float& fTimeDelta); // �߷�
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

