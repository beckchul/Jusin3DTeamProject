#ifndef ClientNetwork_Stage2_h__
#define ClientNetwork_Stage2_h__

#include "Network.h"
#include "Defines.h"

namespace Engine
{
	class CGameObject;
}

class CClientNetwork_Stage2 : public Engine::CNetwork
{
	DECLARE_SINGLETON(CClientNetwork_Stage2)
private:
	explicit CClientNetwork_Stage2();
	virtual ~CClientNetwork_Stage2();

public:
	// Setter
	void SetCurrentScene(SCENEID eScene);

	// ������ ���� �޼����� ���� ������ �Լ���
	virtual void RecvData(void);

	void Message_Create_Player(Engine::NETDATA tData);
	void Message_Delete_Player(int iIndex);
	void Message_Update_Player(Engine::NETDATA tData);

	void Message_Chat(Engine::NETDATA tData);

	void ErrorCheck(void);

	// �� ��
	void Check_Player_Create(void); // �����κ��� �÷��̾ �����϶�� �޼����� �޾Ҵٸ� �÷��̾ ����������
	void Check_Player_Delete(void); // �����κ��� �÷��̾ �����϶�� �޼����� �޾Ҵٸ�

	void Create_Player(int iIndex); // ������ �÷��̾ �����ؼ� �迭�� ����
	void Delete_Player(int iIndex); // ������ �÷��̾ ����

	void Stage_Clear(void); // �������� �Ѿ ��

private:
	bool					m_bClientSetting[MAX_CLIENT_NUM2]; // �������� �÷��̾� �����϶�� �޼����� �޾Ҵ���
	Engine::CGameObject*	m_pPlayerArray[MAX_CLIENT_NUM2]; // �÷��̾� �迭

	_int					m_iIndex = -1;

	Engine::CGameObject*	m_pChat = nullptr; // ä��

	SCENEID		m_eCurrentScene = SCENE_END; // ���� Scene
private:
	virtual void Free(void);
};

#endif