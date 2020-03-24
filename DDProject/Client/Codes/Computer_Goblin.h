#pragma once

#include "DynamicObject.h"

namespace Engine
{
	class CNavMgr;
	class CInfoSubject;
	class CInputDev;
	class CCollisionSphere;
	class CDataManager;
}

class CFrustumManager;
class CCollisionManager;
class CLastManObserver;
class CComputer_Goblin : public CDynamicObject
{
public:
	explicit CComputer_Goblin(LPDIRECT3DDEVICE9 pGraphicDev, _uint uIndex);
	virtual ~CComputer_Goblin();
public:
	virtual HRESULT Ready_GameObject(_vec3 vPosition, vector<_int> vecRamdom);
public:
	// 서버 동기화
	void Server_SetDead(void);
	

	// NavMesh
	void Player_Fall(const float& fTimeDelta); // 중력
	void Player_Check_SecondFloor(void); // 2층으로 갈 수 있는 상태인지 체크

public:
	virtual int FirstUpdate_GameObject(const float& fTimeDelta);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void FirstRender_GameObject(_bool bTrue);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
	void Reset_AI(void);
private:
	void Pattern(void);
	void Change_State(void);
	void Control_PlayerMove(const float& fTimeDelta);
	void Check_Collision(void);
	void Calculate_Radius(void);

	// 자기장
	void Check_Magnetic(void);
private:
	// Observer
	CLastManObserver*			m_pLastManObserver = nullptr;
	CInfoSubject*				m_pInfoSubject = nullptr;
	// NavMesh
	DWORD						m_dwIndex = -1;
	CNavMgr*					m_pNavMgr = nullptr;

	//Reset
	_vec3						m_vecResetPosition;
	// Collision
	CCollisionSphere*			m_pCollisionBody = nullptr;
	CCollisionManager*			m_pCollisionManager = nullptr;
	// Animation
	_int						m_iRenderState = 0;
	_int						m_iPreRenderState = 0;
	_float						m_fDeadTrackPosition = 0.f;				//몬스터 사망 Frame
	_int						m_iDeadTrackIndex = 0;					//몬스터 사망 Index

	LASTMAN_INFO				m_tLastManInfo;
	_bool						m_bFirstUpdate = false;
	_float						m_fDeltaTime = 0.f;

	// Patton
	_float						m_fCount = 0.f;
	_int						m_iPattonCount = 0;
	_float						m_fPattonTime = 0.f;
	_int						m_iRandomPatton = 0;
	_float						m_fXMove = 0.f;
	_float						m_fZMove = 0.f;
	_float						m_fRotationCount = 0.f;
	MONSTER_MAIN_STATE			m_ePattonState = MONSTER_IDLE;
	vector<_int>				m_vecRamdom;
	_int						m_iRamdom = 0;
	_bool						m_bFirstRender = false;
private:
	CFrustumManager*	m_pFrustumMgr = nullptr;
	CDataManager*		m_pDataMgr = nullptr;
	_float				m_fRadius = 0.f;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CComputer_Goblin* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPosition, vector<_int> vecRamdom, _uint uIndex);
private:
	virtual void Free(void);
};

