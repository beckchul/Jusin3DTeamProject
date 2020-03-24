#include "stdafx.h"
#include "ClientNetwork_Stage2.h"

#include "Player.h"
#include "Chat.h"
#include "Object_Manager.h"
#include "InfoSubject.h"

IMPLEMENT_SINGLETON(CClientNetwork_Stage2)

CClientNetwork_Stage2::CClientNetwork_Stage2()
	: m_eCurrentScene(SCENE_END)
	, m_pChat(nullptr)
{
	ZeroMemory(m_bClientSetting, sizeof(bool) * MAX_CLIENT_NUM2);
	ZeroMemory(m_pPlayerArray, sizeof(m_pPlayerArray));

	CInfoSubject::GetInstance()->Add_Data(MESSAGE_LASTMAN_DELETE, &m_iIndex);
}

CClientNetwork_Stage2::~CClientNetwork_Stage2()
{
}

void CClientNetwork_Stage2::SetCurrentScene(SCENEID eScene)
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

void CClientNetwork_Stage2::RecvData(void)
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
			std::packaged_task<void()> task(bind(&CClientNetwork_Stage2::RecvData, this));
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
			break;

		case Engine::NETMSG_SERVER_CREATE_PLAYER: // �÷��̾� ����
			Message_Create_Player(RxData);
			break;

		case Engine::NETMSG_SERVER_DELETE_PLAYER: // �÷��̾� ����
			Message_Delete_Player(RxData.iIndex);
			break;

		case Engine::NETMSG_SERVER_PLAYER_UPDATE: // �÷��̾� ������Ʈ(��ġ)
			Message_Update_Player(RxData);
			break;

		case Engine::NETMSG_SERVER_SEND_CHATTING: // ä��
			Message_Chat(RxData);
			break;
		}
	}
}

void CClientNetwork_Stage2::Message_Create_Player(Engine::NETDATA tData)
{
	m_bClientSetting[tData.iIndex] = true;
}

void CClientNetwork_Stage2::Message_Delete_Player(int iIndex)
{
	m_bClientSetting[iIndex] = false;
}

void CClientNetwork_Stage2::Message_Update_Player(Engine::NETDATA tData)
{
	if (nullptr == m_pPlayerArray[tData.iIndex])
		return;

	dynamic_cast<CPlayer*>(m_pPlayerArray[tData.iIndex])->Set_LocalData(tData);
}

void CClientNetwork_Stage2::Message_Chat(Engine::NETDATA tData)
{
	if (nullptr == m_pChat)
		return;

	dynamic_cast<CChat*>(m_pChat)->Get_ServerChat(tData);
}

void CClientNetwork_Stage2::ErrorCheck(void)
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

void CClientNetwork_Stage2::Check_Player_Create(void)
{
	for (int i = 1; i < MAX_CLIENT_NUM2; ++i)
	{
		if (false == m_bClientSetting[i]) // �����κ��� �����϶�� �޼����� �޾Ҵ���
			continue;

		if (nullptr == m_pPlayerArray[i]) // ���� �ȸ�����ٸ� ������
			Create_Player(i);
	}
}

void CClientNetwork_Stage2::Check_Player_Delete(void)
{
	for (int i = 1; i < MAX_CLIENT_NUM2; ++i)
	{
		if (true == m_bClientSetting[i]) // �����κ��� �����϶�� �޼����� �޾Ҵ���
			continue;

		if (nullptr != m_pPlayerArray[i]) // ���� ���� ���ߴٸ� ����
			Delete_Player(i);
	}
}

void CClientNetwork_Stage2::Create_Player(int iIndex)
{
	++m_iPlayerNum;

	cout << g_iClientIndex << " �� Ŭ�� : " << iIndex << " �� �÷��̾� ���� " << endl;

	Engine::CGameObject* pGameObject = nullptr;

	//pGameObject = CPlayer_Mage::Create(m_pGraphicDev);
	
	if (nullptr == pGameObject)
		return;

	dynamic_cast<CPlayer*>(pGameObject)->SetServerIndex(iIndex);
	m_pPlayerArray[iIndex] = pGameObject;
	m_pPlayerArray[iIndex]->Add_Ref();

	Engine::CObject_Manager::GetInstance()->Add_GameObject(m_eCurrentScene, L"Layer_GameObject", pGameObject);
}

void CClientNetwork_Stage2::Delete_Player(int iIndex)
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

void CClientNetwork_Stage2::Stage_Clear(void)
{
	for (int i = 0; i < MAX_CLIENT_NUM2; ++i)
	{
		Engine::Safe_Release(m_pPlayerArray[i]);
		m_pPlayerArray[i] = nullptr;

		m_iIndex = i;
		CInfoSubject::GetInstance()->Notify_Message(MESSAGE_LASTMAN_DELETE);
	}
}

void CClientNetwork_Stage2::Free(void)
{
	for (int i = 1; i < MAX_CLIENT_NUM2; ++i)
		DWORD dwRefCnt = Engine::Safe_Release(m_pPlayerArray[i]);

	Engine::Safe_Release(m_pChat);

	CInfoSubject::GetInstance()->Remove_Data(MESSAGE_LASTMAN_DELETE, &m_iIndex);

	CNetwork::Free();
}
