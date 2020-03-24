#ifndef ClientNetwork_h__
#define ClientNetwork_h__

#include "Network.h"
#include "Defines.h"

namespace Engine
{
	class CGameObject;
}

class CClientNetwork: public Engine::CNetwork
{
	DECLARE_SINGLETON(CClientNetwork)
private:
	explicit CClientNetwork();
	virtual ~CClientNetwork();

public:
	// Setter
	void SetCurrentScene(SCENEID eScene); 

	// ������ ���� �޼����� ���� ������ �Լ���
	virtual void RecvData(void);
	void ErrorCheck(void);
public:
	
	void Message_Create_Player(Engine::NETDATA tData);
	void Message_Delete_Player(int iIndex);
	void Message_Update_Player(Engine::NETDATA tData);

	void Message_Chat(Engine::NETDATA tData);
	void Message_Tower(Engine::NETDATA tData);
	void Message_CombatStart(void);

	// Stage1
	void Check_Player_Create(void); // �����κ��� �÷��̾ �����϶�� �޼����� �޾Ҵٸ� �÷��̾ ����������
	void Check_Player_Delete(void); // �����κ��� �÷��̾ �����϶�� �޼����� �޾Ҵٸ�

	void Create_Player(int iIndex); // ������ �÷��̾ �����ؼ� �迭�� ����
	void Delete_Player(int iIndex); // ������ �÷��̾ ����

	void Stage_Clear(void); // �������� �Ѿ ��

	void Send_ChangeMessage(_int iBeforePlayer, _int iAfterPlayer);
	void Change_Player(_int iBeforePlayer, _int iAfterPlayer); // �÷��̾� ��ü

	// Stage2
	void Message_Update_Player2(Engine::NETDATA tData);
	void Message_Decal(Engine::NETDATA tData);
	void Message_Kill(Engine::NETDATA tData);

	// �� ��
	void Check_Player_Create2(void); // �����κ��� �÷��̾ �����϶�� �޼����� �޾Ҵٸ� �÷��̾ ����������
	void Check_Player_Delete2(void); // �����κ��� �÷��̾ �����϶�� �޼����� �޾Ҵٸ�

	void Create_Player2(int iIndex); // ������ �÷��̾ �����ؼ� �迭�� ����
	void Delete_Player2(int iIndex); // ������ �÷��̾ ����

	void Stage_Clear2(void); // �������� �Ѿ ��
	void Message_ID(Engine::NETDATA tData);
	void Stage2_State(Engine::NETDATA tData);

	void Create_Stage2Monster(void);

	void Reset_Round(void);

private:
	// Stage1
	_bool					m_bClientSetting[MAX_CLIENT_NUM]; // �������� �÷��̾� �����϶�� �޼����� �޾Ҵ���
	Engine::CGameObject*	m_pPlayerArray[MAX_CLIENT_NUM]; // �÷��̾� �迭

	SCENEID		m_eCurrentScene = SCENE_END; // ���� Scene
	PHASE_TYPE  m_ePhaseType = BUILD_PHASE;

private:
	// Stage2
	_bool							m_bClientSetting2[MAX_CLIENT_NUM2]; // �������� �÷��̾� �����϶�� �޼����� �޾Ҵ���
	_bool							m_bClientIDSetting[MAX_CLIENT_NUM2]; // ID�� ����ߴ���
	Engine::CGameObject*			m_pPlayerArray2[MAX_CLIENT_NUM2]; // ��Ʈ�� �迭
	vector<Engine::CGameObject*>	m_vecAI; // AI�� ����			 

private:
	_int					m_iIndex = -1;
	Engine::CGameObject*	m_pChat = nullptr; // ä��
private:
	virtual void Free(void);
};

#endif