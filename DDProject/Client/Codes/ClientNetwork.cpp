#include "stdafx.h"
#include "ClientNetwork.h"

#include "Player.h"
#include "Player_Mage.h"
#include "Player_Huntress.h"
#include "Player_Monk.h"
#include "Player_Goblin.h"
#include "Computer_Goblin.h"
#include "Chat.h"
#include "Object_Manager.h"
#include "DynamicObject.h"
#include "InfoSubject.h"
#include "DataManager.h"

#include "EffectMgr.h"

IMPLEMENT_SINGLETON(CClientNetwork)

CClientNetwork::CClientNetwork()
	: m_eCurrentScene(SCENE_END)
	, m_pChat(nullptr)
	, m_ePhaseType(BUILD_PHASE)
{
	// Stage1
	ZeroMemory(m_bClientSetting, sizeof(bool) * MAX_CLIENT_NUM);
	ZeroMemory(m_pPlayerArray, sizeof(m_pPlayerArray));

	CInfoSubject::GetInstance()->Add_Data(MESSAGE_PHASECHANGE, &m_ePhaseType);
	CInfoSubject::GetInstance()->Add_Data(MESSAGE_PLAYER_DELETE, &m_iIndex);

	// Stage2
	ZeroMemory(m_bClientSetting2, sizeof(bool) * MAX_CLIENT_NUM2);
	ZeroMemory(m_bClientIDSetting, sizeof(bool) * MAX_CLIENT_NUM2);
	
	ZeroMemory(m_pPlayerArray2, sizeof(m_pPlayerArray2));

	CInfoSubject::GetInstance()->Add_Data(MESSAGE_LASTMAN_DELETE, &m_iIndex);
}

CClientNetwork::~CClientNetwork()
{
}

void CClientNetwork::SetCurrentScene(SCENEID eScene)
{
	// Scene ����
	m_eCurrentScene = eScene;

	// �÷��̾� ���� ��û
	Engine::NETDATA tData;
	ZeroMemory(&tData, sizeof(Engine::NETDATA));
	tData.chKey = 72;

	tData.eMessage = Engine::NETMSG_CLIENT_CREATE_PLAYER;
	SendMsg(tData);

	// Chat ����
	Engine::CGameObject* pGameObject = CChat::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return;

	m_pChat = pGameObject;
	m_pChat->Add_Ref();

	Engine::CObject_Manager::GetInstance()->Add_GameObject(m_eCurrentScene, L"Layer_UI", pGameObject);
}

void CClientNetwork::RecvData(void)
{
	while (nullptr != m_pInstance)
	{
		Engine::NETDATA RxData = RecvMsg();

		ErrorCheck();

		if (72 != RxData.chKey) // ����� �� ���� ���Դ��� Ȯ���ϴ°�.
		{
			std::cout << "�߸��� Ű(72�� �ƴ�) ����" << std::endl;

			/*if (S_OK == InitNetwork(m_pGraphicDev))
			{
				std::packaged_task<void()> task(bind(&CClientNetwork::RecvData, this));
				thread temp(move(task));
				temp.detach();
			}
			break;*/
		}

		switch (RxData.eMessage)
		{
		case Engine::NETMSG_SERVER_SET_INDEX:

			if (-1 != g_iClientIndex)
			{
				cout << "�߸��� ���� �ε��� �ʱ�ȭ" << endl; // ���� Message�� �������� ���´�.
				break;
			}

			g_iClientIndex = RxData.iIndex;		// ���� ���ӽ� �ѹ��� ����
			std::cout << "Ŭ���̾�Ʈ ��ȣ : " << g_iClientIndex << std::endl;

			m_bClientSetting[g_iClientIndex] = true;
			m_bClientSetting2[g_iClientIndex] = true;
			break;

		case Engine::NETMSG_SERVER_CREATE_PLAYER: // �÷��̾� ����
			Message_Create_Player(RxData);
			break;

		case Engine::NETMSG_SERVER_DELETE_PLAYER: // �÷��̾� ����
			Message_Delete_Player(RxData.iIndex);
			break;

		case Engine::NETMSG_SERVER_SEND_CHATTING: // ä��
			Message_Chat(RxData);
			break;

			// Stage1
		case Engine::NETMSG_SERVER_PLAYER_UPDATE: // �÷��̾� ������Ʈ(��ġ)
			Message_Update_Player(RxData);
			break;

		case Engine::NETMSG_SERVER_UPDATE_TOWER: // Ÿ��
			Message_Tower(RxData);
			break;

		case Engine::NETMSG_SERVER_START_COMBAT: // ��������
			Message_CombatStart();
			break;

		case Engine::NETMSG_SERVER_CHANGE_PLAYER: // �÷��̾� ��ü
			Change_Player(RxData.iIndex, RxData.wSubMessage);
			break;

			// Stage2
		case Engine::NETMSG_SERVER_PLAYER_UPDATE2: // �÷��̾� ������Ʈ(��ġ)
			Message_Update_Player2(RxData);
			break;

		case Engine::NETMSG_SERVER_SEND_ID: // �÷��̾� ���̵�
			Message_ID(RxData);
			break;

		case Engine::NETMSG_SERVER_DECAL:
			Message_Decal(RxData);
			break;

		case Engine::NETMSG_SERVER_ROUND2_STATE:
			Stage2_State(RxData);
			break;

		case Engine::NETMSG_SERVER_KILL:
			Message_Kill(RxData);
			break;
		}
	}
}

void CClientNetwork::Message_Create_Player(Engine::NETDATA tData)
{
	m_bClientSetting[tData.iIndex] = true;
	m_bClientSetting2[tData.iIndex] = true;
}

void CClientNetwork::Message_Delete_Player(int iIndex)
{
	m_bClientSetting[iIndex] = false;
	m_bClientSetting2[iIndex] = false;
	m_bClientIDSetting[iIndex] = false;
}

void CClientNetwork::Message_Update_Player(Engine::NETDATA tData)
{
	if (nullptr == m_pPlayerArray[tData.iIndex])
		return;

	dynamic_cast<CPlayer*>(m_pPlayerArray[tData.iIndex])->Set_LocalData(tData);
}

void CClientNetwork::Message_Chat(Engine::NETDATA tData)
{
	if (nullptr == m_pChat)
		return;

	dynamic_cast<CChat*>(m_pChat)->Get_ServerChat(tData);
}

void CClientNetwork::Message_Tower(Engine::NETDATA tData)
{
	if (nullptr == m_pPlayerArray[tData.iIndex])
		return;

	dynamic_cast<CPlayer*>(m_pPlayerArray[tData.iIndex])->Set_TowerData(tData);
}

void CClientNetwork::Message_CombatStart(void)
{
	m_ePhaseType = COMBAT_PHASE;

	CInfoSubject::GetInstance()->Notify_Message(MESSAGE_PHASECHANGE);

	// �ΰ� : ���� 1�������� ũ����Ż �������� Pos�־�����ű�, ������������ �б� �ʿ�
	if(g_eCurrentScene == SCENE_STAGE)
		Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_GameStart_Flare", &_vec3(0.f, 22.f, 0.f));
}

void CClientNetwork::ErrorCheck(void)
{
	int iErrorType = WSAGetLastError();
	switch (iErrorType)
	{
	case 0:
		// printf("����\n");
		break;

	case WSAENOTCONN:	// ������ ����Ǿ� ���� ���� ��
		printf("-- 1�� ����\n");
		break;

	case WSAESHUTDOWN:	// ������ ����Ǿ��� ��
		printf("-- 2�� ����\n");
		break;

	case WSANOTINITIALISED:
		// �������� WSAStartup �Լ��� ȣ������ ����� ����ϱ� ���� �߻��ؾ��մϴ�.
		printf("-- 3�� ����\n");
		break;

	case WSAENETDOWN:
		// ��Ʈ��ũ ���� �ý��ۿ� ������ �߻��߽��ϴ�.
		printf("-- 4�� ����\n");
		break;

	case WSAEFAULT:
		// ���� �Ķ���ʹ� ������ ����� �ּ� ������ ��ȿ �κп� ���Ե��� �ʴ´�.
		printf("-- 5�� ����\n");
		break;

	case WSAEINTR:
		// ����)ȣ���� ����߽��ϴ� WSACancelBlockingCall .
		printf("-- 6�� ����\n");
		break;

	case WSAEINPROGRESS:
		// ��ŷ ���� v1.1�� ���� ���� ���̰ų�, ���� ���� ���̴��� �ݹ� �Լ��� ������ ó���ϰ� �ֽ��ϴ�.
		printf("-- 7�� ����\n");
		break;

	case WSAENETRESET:
		// ���� ���� ������ ���, 
		// �� ������ �����̷� ���� �ı��Ǿ����� ��Ÿ���ϴ�.
		// ���� ���� �۾��� ����Ǵ� ���� ������ ���� Ȱ��.
		// ������ �׷� ������ ���, �� ������ �� ���ִ� 
		// �Ⱓ�� ����Ǿ����� ��Ÿ���ϴ�.
		printf("-- 8�� ����\n");
		break;

	case WSAENOTSOCK:
		// ����ڴ� ������ �ƴϴ�.
		printf("-- 9�� ����\n");
		break;

	case WSAEOPNOTSUPP:
		// MSG_OOB�� ���� �Ǿ�����, ���� Ÿ�� SOCK_STREAM�� ���� 
		// ��Ʈ�� ��Ÿ�� �ƴ�, OOB �����ʹ��� ���ϰ� ���õ� 
		// ��� �����ο��� �������� �ʽ��ϴ�, 
		// ������ �ܹ��� �� �۾��� ���� �����մϴ�.
		printf("-- 10�� ����\n");
		break;
	case WSAEWOULDBLOCK:
		// ������ ���ŷ���� ǥ�õǸ�, 
		// ���� ������ �����ϴ� ���Դϴ�.
		//printf(" - No Message\n");
		//printf("-- 11�� ���� : \n");
		break;

	case WSAEMSGSIZE:
		// �� �޽����� ������ ���ۿ� ���� �ʹ� ũ�� �� �Ƚ��ϴ�.
		printf("-- 11�� ����\n");
		break;

	case WSAEINVAL:
		// ���Ͽ� ������� ���� ���� , �Ǵ� �� ������ �÷��װ�
		// ���� ��, �Ǵ� MSG_OOB�� SO_OOBINLINE ����ϰų� 
		// ���� ���� ���� (���� ����Ʈ ��Ʈ�� ���Ͽ� ����) 
		// LEN�� ���� �Ǵ� �����̾���.
		printf("-- 12�� ����\n");
		break;

	case WSAECONNABORTED:
		// ���� ȸ�� ���� Ÿ�� �ƿ� �Ǵ� �ٸ� ������ ������״�.
		// �� �̻� ����� �� ���� ���� ���α׷��� ������ �ݽ��ϴ� �����ϴ�.
		printf("-- 13�� ����\n");
		break;

	case WSAETIMEDOUT:
		// ������ ��Ʈ��ũ ��ַ� ���� �Ǵ� 
		// �Ǿ� �ý����� �������� ���߱� ������ �����Ǿ����ϴ�.
		printf("-- 14�� ����\n");
		break;

	case WSAECONNRESET:
		// ���� ȸ���� ���� �Ǵ� �ҿ��� �ݱ⸦ �����ϴ� 
		// ���� ���� ���� ���µǾ���.
		// �� �̻� ����� �� ���� ���� ���α׷��� ������ �ݽ��ϴ� �����ϴ�.
		// �� UDP ������ �׷� ���Ͽ����� ������ 
		// ������ ���� �۾��� ICMP "��Ʈ�� ������ �� ����"�޽����� 
		// ��� ���� ��Ÿ���ϴ�.
		printf("���ݿ��� ������ �������ϴ�.\n");
		break;

	default:
		printf("Unknown Error.\n");
		break;
	}
}

void CClientNetwork::Check_Player_Create(void)
{
	for (int i = 1; i < MAX_CLIENT_NUM; ++i)
	{
		if (false == m_bClientSetting[i]) // �����κ��� �����϶�� �޼����� �޾Ҵ���
			continue;

		if (nullptr == m_pPlayerArray[i]) // ���� �ȸ�����ٸ� ������
			Create_Player(i);
	}
}

void CClientNetwork::Check_Player_Delete(void)
{
	for (int i = 1; i < MAX_CLIENT_NUM; ++i)
	{
		if (true == m_bClientSetting[i]) // �����κ��� �����϶�� �޼����� �޾Ҵ���
			continue;

		if (nullptr != m_pPlayerArray[i]) // ���� ���� ���ߴٸ� ����
			Delete_Player(i);
	}
}

void CClientNetwork::Create_Player(int iIndex)
{
	++m_iPlayerNum;

	cout << g_iClientIndex << " �� Ŭ�� : " << iIndex << " �� �÷��̾� ���� " << endl;

	// ToDo : � ĳ�������� SubMessage �޾Ƽ� ó���ؾ� ��
	Engine::CGameObject* pGameObject = nullptr;
		
	switch (iIndex)
	{
	case 1:
		pGameObject = CPlayer_Monk::Create(m_pGraphicDev);
		break;

	case 2:
		pGameObject = CPlayer_Monk::Create(m_pGraphicDev);
		break;

	case 3:
		pGameObject = CPlayer_Mage::Create(m_pGraphicDev);
		break;

	case 4:
		pGameObject = CPlayer_Monk::Create(m_pGraphicDev);
		break;

	default:
		pGameObject = CPlayer_Monk::Create(m_pGraphicDev);
		break;
	}
		
	if (nullptr == pGameObject)
		return;

	dynamic_cast<CPlayer*>(pGameObject)->SetServerIndex(iIndex);
	m_pPlayerArray[iIndex] = pGameObject;
	m_pPlayerArray[iIndex]->Add_Ref();

	Engine::CObject_Manager::GetInstance()->Add_GameObject(m_eCurrentScene, L"Layer_GameObject", pGameObject);
}

void CClientNetwork::Delete_Player(int iIndex)
{
	--m_iPlayerNum;

	cout << iIndex << "�� Ŭ���̾�Ʈ Ż�� " << endl;

	if (nullptr != m_pPlayerArray[iIndex])
		dynamic_cast<CPlayer*>(m_pPlayerArray[iIndex])->Set_ServerExit(); // Player Delete

	Engine::Safe_Release(m_pPlayerArray[iIndex]);
	m_pPlayerArray[iIndex] = nullptr;

	m_iIndex = iIndex;
	CInfoSubject::GetInstance()->Notify_Message(MESSAGE_PLAYER_DELETE);
}

void CClientNetwork::Stage_Clear(void)
{
	for (int i = 0; i < MAX_CLIENT_NUM; ++i)
	{
		Engine::Safe_Release(m_pPlayerArray[i]);
		m_pPlayerArray[i] = nullptr;

		m_iIndex = i;
		CInfoSubject::GetInstance()->Notify_Message(MESSAGE_PLAYER_DELETE);
	}

	Engine::Safe_Release(m_pChat);
	m_pChat = nullptr;
}

void CClientNetwork::Send_ChangeMessage(_int iBeforePlayer, _int iAfterPlayer)
{
	if (nullptr == m_pPlayerArray[iBeforePlayer] || nullptr == m_pPlayerArray[iAfterPlayer])
		return;

	Engine::NETDATA tData;
	ZeroMemory(&tData, sizeof(Engine::NETDATA));
	tData.chKey = 72;
	tData.eMessage = NETMSG_CLIENT_CHANGE_PLAYER;
	tData.iIndex = iBeforePlayer; // �� ��ȣ
	tData.wSubMessage = iAfterPlayer; // �ٲ� ��ȣ

	SendMsg(tData);

	Change_Player(iBeforePlayer, iAfterPlayer);
}

void CClientNetwork::Change_Player(_int iBeforePlayer, _int iAfterPlayer)
{
	// iBeforePlayer �÷��̾ iAfterPlayer �÷��̾ �Ƕ�
	if (nullptr == m_pPlayerArray[iBeforePlayer] || nullptr == m_pPlayerArray[iAfterPlayer])
		return;

	dynamic_cast<CPlayer*>(m_pPlayerArray[iBeforePlayer])->Leave_Player();
	dynamic_cast<CPlayer*>(m_pPlayerArray[iAfterPlayer])->Change_Player(iAfterPlayer);
}

// Stage2
void CClientNetwork::Message_Update_Player2(Engine::NETDATA tData)
{
	if (nullptr == m_pPlayerArray2[tData.iIndex])
		return;

	dynamic_cast<CPlayer_Goblin*>(m_pPlayerArray2[tData.iIndex])->Set_LocalData(tData);
}

void CClientNetwork::Message_Decal(Engine::NETDATA tData)
{
	if (nullptr == m_pPlayerArray2[tData.iIndex])
		return;

	dynamic_cast<CPlayer_Goblin*>(m_pPlayerArray2[tData.iIndex])->Create_Decal(tData.wSubMessage);
}

void CClientNetwork::Message_Kill(Engine::NETDATA tData)
{
	if (tData.wSubMessage == 0) // �÷��̾� ų
	{
		if (tData.iIndex >= MAX_CLIENT_NUM2 - 1)
			return;

		dynamic_cast<CPlayer_Goblin*>(m_pPlayerArray2[tData.iHighAnimation])->Server_SetDead();
	}
	else if (tData.wSubMessage == 1) // AIų
	{
		if ((_uint)tData.iIndex >= m_vecAI.size())
			return;

		dynamic_cast<CPlayer_Goblin*>(m_pPlayerArray2[tData.iIndex])->Create_Spotlight();
		dynamic_cast<CComputer_Goblin*>(m_vecAI[tData.iHighAnimation])->Server_SetDead();
	}
}

void CClientNetwork::Check_Player_Create2(void)
{
	for (int i = 1; i < MAX_CLIENT_NUM2; ++i)
	{
		if (false == m_bClientSetting2[i]) // �����κ��� �����϶�� �޼����� �޾Ҵ���
			continue;

		if (false == m_bClientIDSetting[i]) // ���̵� �����ߴ���
			continue;

		if (nullptr == m_pPlayerArray2[i]) // ���� �ȸ�����ٸ� ������
			Create_Player2(i);
	}
}

void CClientNetwork::Check_Player_Delete2(void)
{
	for (int i = 1; i < MAX_CLIENT_NUM2; ++i)
	{
		if (true == m_bClientSetting2[i]) // �����κ��� �����϶�� �޼����� �޾Ҵ���
			continue;

		if (nullptr != m_pPlayerArray2[i]) // ���� ���� ���ߴٸ� ����
			Delete_Player2(i);
	}
}

void CClientNetwork::Create_Player2(int iIndex)
{
	++m_iPlayerNum;

	cout << g_iClientIndex << " �� Ŭ�� : " << iIndex << " �� �÷��̾� ���� " << endl;

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CPlayer_Goblin::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return;

	dynamic_cast<CPlayer_Goblin*>(pGameObject)->SetServerIndex(iIndex);
	m_pPlayerArray2[iIndex] = pGameObject;
	m_pPlayerArray2[iIndex]->Add_Ref();

	Engine::CObject_Manager::GetInstance()->Add_GameObject(m_eCurrentScene, L"Layer_GameObject", pGameObject);

	CInfoSubject::GetInstance()->Notify_Message(MESSAGE_LASTMAN_PLAYER_ADD);
}

void CClientNetwork::Delete_Player2(int iIndex)
{
	--m_iPlayerNum;

	cout << iIndex << "�� Ŭ���̾�Ʈ Ż�� " << endl;

	if (nullptr != m_pPlayerArray2[iIndex])
		dynamic_cast<CPlayer_Goblin*>(m_pPlayerArray2[iIndex])->Set_ServerExit(); // Player Delete

	Engine::Safe_Release(m_pPlayerArray2[iIndex]);
	m_pPlayerArray2[iIndex] = nullptr;

	m_iIndex = iIndex;
	CInfoSubject::GetInstance()->Notify_Message(MESSAGE_LASTMAN_DELETE);
}

void CClientNetwork::Stage_Clear2(void)
{
	for (int i = 0; i < MAX_CLIENT_NUM2; ++i)
	{
		Engine::Safe_Release(m_pPlayerArray2[i]);
		m_pPlayerArray2[i] = nullptr;

		m_iIndex = i;
		CInfoSubject::GetInstance()->Notify_Message(MESSAGE_LASTMAN_DELETE);
	}

	Engine::Safe_Release(m_pChat);
	m_pChat = nullptr;
}

void CClientNetwork::Message_ID(Engine::NETDATA tData)
{
	CDataManager::GetInstance()->Set_Player_ID(tData.iIndex, string(tData.szChatMessage));

	// ���̵� ����� �ڸ� Stage2 ���� ����

	m_bClientIDSetting[tData.iIndex] = true;
}

void CClientNetwork::Stage2_State(Engine::NETDATA tData)
{
	if (tData.wSubMessage == 0) // Round_Wait
	{
		if (m_bClientIDSetting[g_iClientIndex] == TRUE)
		{
			g_iRound++;

			if (g_iRound == 1)
				Create_Stage2Monster();
			else
				Reset_Round();

			g_eLastMan_Progress = LASTMAN_ROUND_WAIT;
		}
	}
}

void CClientNetwork::Create_Stage2Monster(void)
{
	Engine::CGameObject*      pGameObject = nullptr;
	
	vector< vector<int> > vecAIPatturn = CDataManager::GetInstance()->Get_AI_Pattern();
	vector<_vec3> vecAIPos = CDataManager::GetInstance()->Get_AI_Position();

	for (_uint i = 0; i < vecAIPos.size(); ++i)
	{
		vecAIPos[i].y += 3.f;
		pGameObject = CComputer_Goblin::Create(m_pGraphicDev, vecAIPos[i], vecAIPatturn[i], i);
		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject);
		CInfoSubject::GetInstance()->Notify_Message(MESSAGE_LASTMAN_COMPUTER_ADD);

		m_vecAI.push_back(pGameObject);
		pGameObject->Add_Ref();
	}

	/*cout << "���� : " << vecAIPatturn[0][0] << " " << vecAIPatturn[0][1] << " " << vecAIPatturn[0][2] << endl;
	cout << "��ġ : " << vecAIPos[0].x << " " << vecAIPos[0].y << " " << vecAIPos[0].z << endl;*/
}

void CClientNetwork::Reset_Round(void)
{
	m_pPlayerArray2[MAX_CLIENT_NUM2];

	for (size_t i = 0; i < m_vecAI.size(); ++i)
		dynamic_cast<CComputer_Goblin*>(m_vecAI[i])->Reset_AI();

	for (int i = 0; i < MAX_CLIENT_NUM2; ++i)
	{
		if (nullptr == m_pPlayerArray2[i])
			continue;

		dynamic_cast<CPlayer_Goblin*>(m_pPlayerArray2[i])->ResetPlayer();

	}
}

void CClientNetwork::Free(void)
{
	// Stage1
	for (int i = 1; i < MAX_CLIENT_NUM; ++i)
		DWORD dwRefCnt = Engine::Safe_Release(m_pPlayerArray[i]);

	Engine::Safe_Release(m_pChat);

	CInfoSubject::GetInstance()->Remove_Data(MESSAGE_PHASECHANGE, &m_ePhaseType);
	CInfoSubject::GetInstance()->Remove_Data(MESSAGE_PLAYER_DELETE, &m_iIndex); 

	// Stage2
	for (int i = 1; i < MAX_CLIENT_NUM2; ++i)
		DWORD dwRefCnt = Engine::Safe_Release(m_pPlayerArray2[i]);
	
	for (_uint i = 0; i < m_vecAI.size(); ++i)
	{
		Engine::Safe_Release(m_vecAI[i]);
	}
	CInfoSubject::GetInstance()->Remove_Data(MESSAGE_LASTMAN_DELETE, &m_iIndex);

	CNetwork::Free();
}
