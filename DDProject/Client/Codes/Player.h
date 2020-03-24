#pragma once

#include "Defines.h"
#include "DynamicObject.h"

namespace Engine
{
	class CNavMgr;
	class CCollisionSphere;
	class CInputDev;
	class CInfoSubject;
	class CEffect;
}

class CClientNetwork;
class CCollisionManager;
class CCamera_Manager;
class CWeapon;

class CTower;
class CTowerRadius;
class CText_Tower;
class CMousePointer;
class CTowerRange;
class CTowerRange2;

class CPlayer : public CDynamicObject
{
protected:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer(void);
public:
	// Setter
	void Set_LocalData(Engine::NETDATA tData) { m_tLocalData = tData; }
	void SetServerIndex(int iIndex);
	void Set_TowerData(Engine::NETDATA tData);
	void Set_Dead(void) { m_pPlayerInfo->tUnitInfo.bDead = true; }
	void Set_ServerExit(void) { m_bExitServer = true; }

protected:
	CCollisionManager*			m_pCollisionManager = nullptr;
	_bool						m_bFirstFrame = false;
protected:
	Engine::CInputDev*			m_pInputDev = nullptr;
	Engine::CInfoSubject*		m_pInfoSubject = nullptr;
	Engine::CCollisionSphere*	m_pCollisionBody = nullptr;			// 몸체 충돌 구
protected:
	//PLAYER_MAIN_STATE			m_eMainState = PLAYER_END;			// 메인 플레이어 상태
	_matrix*					m_pWeaponMatrix = nullptr;			// 플레이어의 무기(손) 행렬
	_int						m_iAttackCount = 0;					// 공격 타격 횟수
	_float						m_fTrunCount = 0.f;					// 플레이어 회전 관련 카운트
	_float						m_fAttackDelay = 0.f;				// 공격 딜레이
	_float						m_fHealingCount = 0.f;				// Heal 관련 카운트
	_float						m_fHealingHp = 0.f;					// Heal 체력 증가 변수
	_float						m_fHealingMp = 0.f;					// Heal 마나 소모 변수
	_float						m_fDeadTime = 0.f;					// 플레이어 사망 시간 변수
	_float						m_fDeadTrackPosition = 0.f;			// 플레이어 사망 Frame
	_int						m_iDeadTrackIndex = 0;				// 플레이어 사망 Index
protected:
	_int						m_iHighRenderState = 0;				//상체 애니메이션 변수
	_int						m_iPreHighRenderState = 0;			//상체 애니메이션 상태관리 변수
	_int						m_iLowRenderState = 0;				//하체 애니메이션 변수
	_int						m_iLowPreRenderState = 0;			//하체 애니메이션 상태관리 변수
protected:
	const char*					m_pLowFilterBone[3];				//하체만 작동하기 위한 변수
	const char*					m_pHighFilterBone[3];				//상체만 작동하기 위한 변수
protected:
	PLAYER_INFO*				m_pPlayerInfo = nullptr;
	// Weapon
	CWeapon*					m_pWeapon = nullptr;
protected:
	// Server
	int							m_iClinetNum = 0;
	Engine::NETDATA				m_tLocalData;
	bool						m_bMyClient = false;
	bool						m_bExitServer = false;

	_vec3						m_vecTowerPos;
	_vec3						m_vecPick;
	DWORD						m_dwNavOption;

	_vec4						m_vecColor; // 컬러 조절하기위함.
	_vec4						m_vecTransColor; // 컬러조절하기위함.
	bool						m_bColorAdd = false; // 컬러 조절하기위함.
	float						m_fColorTime = 0.f; // 컬러 조절하기위함.
	float						m_fColorLimitTime = 0.f; //컬러 조절하기위함.

protected:
	bool						m_bFirstUpdate = false;
protected:
	// NavMesh
	DWORD						m_dwIndex = -1;
	// Tower
	_bool						m_bAbleToBuild = false;
	TOWER_TYPE					m_eBuildTower = TOWER_TYPE_END;
	_float						m_fSummoningTime = 0.f; // 타워 소환 시간
	_float						m_fWaitingTime = 0.f; // 다른 플레이어 AttachCam 기다리는 시간
private:
	CGameObject*				m_pPreviewTower = nullptr;
	CTowerRadius*				m_pTowerRadius = nullptr;
	CText_Tower*				m_pText_Tower = nullptr;
	CTowerRange*				m_pTowerRange = nullptr;
	CTowerRange2*				m_pTowerRange2 = nullptr;
protected:
	CMousePointer*				m_pMousePointer = nullptr;

protected:	// Minkyeong
	list<CEffect*> m_pHealingEffect;	// Aura
	list<CEffect*> m_pOwn_HealingEffect;	// OwnHealing(자가치유)
	list<CEffect*> m_pSummoingTowerEffect;

protected:
	virtual void Change_State(void) PURE;
	virtual void KeyCheck(const float& fTimeDelta) PURE;
	virtual void Set_Collision(void)PURE;
	virtual void Play_Animation (const float& fTimeDelta);
	void Player_Jump(const float& fTimeDelta); // 점프
	void Control_PlayerMove(const float& fTimeDelta);
	void Check_Collision(void);
	void Move_MouseAngle(void);
	void Healing(void);
	void Hit_UI(const _float& fExistTime);
protected:
	// Tower
	void Mouse_Pick(void);
	void Preview_Tower(const float& fTimeDelta); // 미리보기
	void Preview_Tower2(void); // 미리보기2
	void Create_PreviewTower(TOWER_TYPE eType); // 미리보기 타워 만들기
	void Create_TowerUI(void);
	void Change_TowerColor(_vec4& vecColor); // 타워 색 바꾸기
	void AbleToBuildTower(const DWORD dwOption);
	void Cancel_Tower(void); // 취소
	void Summoning_Tower(const float& fTimeDelta); // 타워 소환중...
	void Build_Tower(void); // 건설
	void Delete_TowerUI(void);
	POINT WorldToScreen(_vec3 vecPos);
protected:
	// Observer
	void Observer_AddData(void);
	void Observer_Update(void);
	void Observer_RemoveData(void);
protected:
	// NavMesh
	void Player_Fall(const float& fTimeDelta); // 중력
	void Player_Check_SecondFloor(void); // 2층으로 갈 수 있는 상태인지 체크
	// Server
public:
	void Leave_Player();
	void Change_Player(int iAfterPlayer);
protected:
	void SendLocalData(void); // 자신(플레이어)의 정보를 서버로 보냄
	void Send_TowerData(_vec3 vecPos, DWORD dwOption); // 현재 Preview 중인 타워의 정보
	void Ready_Combat(void); // 전투 준비 완료


private:
	CNavMgr*			m_pNavMgr = nullptr;
	CCamera_Manager*	m_pCamMgr = nullptr;

protected:
	virtual void Free(void);
};

