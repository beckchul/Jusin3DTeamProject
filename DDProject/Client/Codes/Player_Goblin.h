#pragma once

#include "DynamicObject.h"

namespace Engine
{
	class CNavMgr;
	class CInfoSubject;
	class CInputDev;
	class CCollisionSphere;
	class CEffect;
	class CDataManager;
}

class CPlayer_ID;
class CClientNetwork;
class CCollisionManager;
class CFrustumManager;
class CLastManObserver;
class CPlayer_Goblin : public CDynamicObject
{
public:
	explicit CPlayer_Goblin(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer_Goblin();
public:
	virtual HRESULT Ready_GameObject();
	// 서버 동기화
	void Server_SetDead(void);
	void Create_Spotlight(void);

	// Setter
public:
	void Set_LocalData(Engine::NETDATA tData) { m_tLocalData = tData; }
	void SetServerIndex(int iIndex);
	void Set_ServerExit(void) { m_bExitServer = true; }
	void SendLocalData(void); // 자신(플레이어)의 정보를 서버로 보냄
public:
	// NavMesh
	void Player_Fall(const float& fTimeDelta); // 중력
	void Player_Check_SecondFloor(void); // 2층으로 갈 수 있는 상태인지 체크

	 // Observer
	void Observer_AddData(void);
	void Observer_Update(void);
	void Observer_RemoveData(void);
public:
	virtual int FirstUpdate_GameObject(const float& fTimeDelta);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void FirstRender_GameObject(_bool bTrue);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	
	void Random_Position(void);
	void Change_State(void);
	void KeyCheck(const float& fTimeDelta);
	void Control_PlayerMove(const float& fTimeDelta);
	void Check_Collision(void);
	void Create_ID(void);
	void Show_ID(const float & fTimeDelta);
	void Calculate_Radius(void);
	// Decal
	void Send_Decal(WORD iDecalNum);

	// 자기장
	void Check_Magnetic(void);
public:
	void Create_Decal(WORD iDecalNum);
public:
	void ResetPlayer(void);
private:
	// Observer
	CLastManObserver*			m_pLastManObserver = nullptr;
	// Server
	Engine::NETDATA				m_tLocalData;
	_int						m_iClinetNum = -1;
	_bool						m_bMyClient = false;
	_bool						m_bExitServer = false;
	CClientNetwork*				m_pClinetNet = nullptr;

	// NavMesh
	DWORD						m_dwIndex = -1;
	CNavMgr*					m_pNavMgr = nullptr;

	// Collision
	CCollisionSphere*			m_pCollisionBody = nullptr;
	CCollisionSphere*			m_pCollsionWeapon = nullptr;
	
	CCollisionManager*			m_pCollisionManager = nullptr;
	// Animation
	
	_int						m_iPreRenderState = 0;
	_float						m_fDeadTrackPosition = 0.f;				//몬스터 사망 Frame
	_int						m_iDeadTrackIndex = 0;					//몬스터 사망 Index

	_float						m_fAttackDelay = 0.f;
	_float						m_fJumpDelay = 0.f;
	_float						m_fAvoidDelay = 0.f;
	_float						m_fSlowDelay = 1.f;
	_float						m_fSlowTime = 1.f;
	_float						m_fSlowReadyTime = 1.f;
	_float						m_fSpeedRun = 0.f;

	CInfoSubject*				m_pInfoSubject = nullptr;
	CInputDev*					m_pInputDev = nullptr;

	// ID
	CPlayer_ID*					m_pPlayerID = nullptr;
	_bool						m_bShowID = true;

	LASTMAN_INFO*				m_pLastManInfo = nullptr;
	_bool						m_bFirstUpdate = false;
	_float						m_fDeltaTime = 0.f; 

	vector<TCHAR*>				m_vecDecalName;
	_float						m_fDelcalCoolDown = 0.f;

	//Effect
	list<CEffect*>				m_Effectlist;
private:
	CFrustumManager*			m_pFrustumMgr = nullptr;
	_float						m_fRadius = 0.f;
	Engine::CEffect*			m_pTempDecal;
	CDataManager*				m_pDataMgr = nullptr;

	static _int					m_bMyClientDead;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CPlayer_Goblin* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

