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
	Engine::CCollisionSphere*	m_pCollisionBody = nullptr;			// ��ü �浹 ��
protected:
	//PLAYER_MAIN_STATE			m_eMainState = PLAYER_END;			// ���� �÷��̾� ����
	_matrix*					m_pWeaponMatrix = nullptr;			// �÷��̾��� ����(��) ���
	_int						m_iAttackCount = 0;					// ���� Ÿ�� Ƚ��
	_float						m_fTrunCount = 0.f;					// �÷��̾� ȸ�� ���� ī��Ʈ
	_float						m_fAttackDelay = 0.f;				// ���� ������
	_float						m_fHealingCount = 0.f;				// Heal ���� ī��Ʈ
	_float						m_fHealingHp = 0.f;					// Heal ü�� ���� ����
	_float						m_fHealingMp = 0.f;					// Heal ���� �Ҹ� ����
	_float						m_fDeadTime = 0.f;					// �÷��̾� ��� �ð� ����
	_float						m_fDeadTrackPosition = 0.f;			// �÷��̾� ��� Frame
	_int						m_iDeadTrackIndex = 0;				// �÷��̾� ��� Index
protected:
	_int						m_iHighRenderState = 0;				//��ü �ִϸ��̼� ����
	_int						m_iPreHighRenderState = 0;			//��ü �ִϸ��̼� ���°��� ����
	_int						m_iLowRenderState = 0;				//��ü �ִϸ��̼� ����
	_int						m_iLowPreRenderState = 0;			//��ü �ִϸ��̼� ���°��� ����
protected:
	const char*					m_pLowFilterBone[3];				//��ü�� �۵��ϱ� ���� ����
	const char*					m_pHighFilterBone[3];				//��ü�� �۵��ϱ� ���� ����
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

	_vec4						m_vecColor; // �÷� �����ϱ�����.
	_vec4						m_vecTransColor; // �÷������ϱ�����.
	bool						m_bColorAdd = false; // �÷� �����ϱ�����.
	float						m_fColorTime = 0.f; // �÷� �����ϱ�����.
	float						m_fColorLimitTime = 0.f; //�÷� �����ϱ�����.

protected:
	bool						m_bFirstUpdate = false;
protected:
	// NavMesh
	DWORD						m_dwIndex = -1;
	// Tower
	_bool						m_bAbleToBuild = false;
	TOWER_TYPE					m_eBuildTower = TOWER_TYPE_END;
	_float						m_fSummoningTime = 0.f; // Ÿ�� ��ȯ �ð�
	_float						m_fWaitingTime = 0.f; // �ٸ� �÷��̾� AttachCam ��ٸ��� �ð�
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
	list<CEffect*> m_pOwn_HealingEffect;	// OwnHealing(�ڰ�ġ��)
	list<CEffect*> m_pSummoingTowerEffect;

protected:
	virtual void Change_State(void) PURE;
	virtual void KeyCheck(const float& fTimeDelta) PURE;
	virtual void Set_Collision(void)PURE;
	virtual void Play_Animation (const float& fTimeDelta);
	void Player_Jump(const float& fTimeDelta); // ����
	void Control_PlayerMove(const float& fTimeDelta);
	void Check_Collision(void);
	void Move_MouseAngle(void);
	void Healing(void);
	void Hit_UI(const _float& fExistTime);
protected:
	// Tower
	void Mouse_Pick(void);
	void Preview_Tower(const float& fTimeDelta); // �̸�����
	void Preview_Tower2(void); // �̸�����2
	void Create_PreviewTower(TOWER_TYPE eType); // �̸����� Ÿ�� �����
	void Create_TowerUI(void);
	void Change_TowerColor(_vec4& vecColor); // Ÿ�� �� �ٲٱ�
	void AbleToBuildTower(const DWORD dwOption);
	void Cancel_Tower(void); // ���
	void Summoning_Tower(const float& fTimeDelta); // Ÿ�� ��ȯ��...
	void Build_Tower(void); // �Ǽ�
	void Delete_TowerUI(void);
	POINT WorldToScreen(_vec3 vecPos);
protected:
	// Observer
	void Observer_AddData(void);
	void Observer_Update(void);
	void Observer_RemoveData(void);
protected:
	// NavMesh
	void Player_Fall(const float& fTimeDelta); // �߷�
	void Player_Check_SecondFloor(void); // 2������ �� �� �ִ� �������� üũ
	// Server
public:
	void Leave_Player();
	void Change_Player(int iAfterPlayer);
protected:
	void SendLocalData(void); // �ڽ�(�÷��̾�)�� ������ ������ ����
	void Send_TowerData(_vec3 vecPos, DWORD dwOption); // ���� Preview ���� Ÿ���� ����
	void Ready_Combat(void); // ���� �غ� �Ϸ�


private:
	CNavMgr*			m_pNavMgr = nullptr;
	CCamera_Manager*	m_pCamMgr = nullptr;

protected:
	virtual void Free(void);
};

