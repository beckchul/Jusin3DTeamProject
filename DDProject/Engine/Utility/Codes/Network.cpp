#include "Network.h"

Engine::CNetwork::CNetwork()
	: m_iPlayerNum(0)
	, m_pGraphicDev(nullptr)
{
}

Engine::CNetwork::~CNetwork()
{
}

HRESULT Engine::CNetwork::InitNetwork(LPDIRECT3DDEVICE9 pGraphicDev)
{
	// ���� ���̺귯�� �ʱ�ȭ
	WSADATA wsaData;
	int token = WSAStartup(WINSOCK_VERSION, &wsaData);

	// ���� ����
	socket_client = socket(AF_INET, SOCK_STREAM, 0);

	// ���� �ּ� ����
	SOCKADDR_IN servAddr = { 0 };
	servAddr.sin_family = AF_INET;

	// ����ȣ PC IP : 199
	HANDLE m_hFile = CreateFile(L"../Resources/Data/ServerData/Server.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	char Temp[32] = { 0, };

	DWORD dwByte = 0;

	ReadFile(m_hFile, Temp, sizeof(char) * 32, &dwByte, NULL);

	inet_pton(AF_INET, Temp, &(servAddr.sin_addr));
	inet_pton(AF_INET, "127.0.0.1", &(servAddr.sin_addr)); // Loop Back
	//inet_pton(AF_INET, "192.168.1.223", &(servAddr.sin_addr)); // ��
	//inet_pton(AF_INET, "192.168.1.4", &(servAddr.sin_addr)); //��
	//inet_pton(AF_INET, "192.168.1.170", &(servAddr.sin_addr)); //��
	
	CloseHandle(m_hFile);

	servAddr.sin_port = htons(6872);

	// ���� ����
	if (connect(socket_client, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		closesocket(socket_client);
		MSG_BOX(L"������ ������ �� �����ϴ�.");
		return SOCKET_ERROR;
	}

	m_pGraphicDev = pGraphicDev;
	m_pGraphicDev->AddRef();

	return S_OK;
}

void Engine::CNetwork::SendMsg(Engine::NETDATA tData)
{
	send(socket_client, (char*)&tData, sizeof(tData), 0);
}

Engine::NETDATA Engine::CNetwork::RecvMsg(void)
{
	const int iSize = sizeof(NETDATA);
	char buffer[iSize] = { 0, };
	NETDATA* RxData;

	int strLen = recv(socket_client, buffer, iSize, 0);
	if (0 >= strLen)
	{
		closesocket(socket_client);
		//std::cout << "������ ���� ����Ǿ����ϴ�." << endl;
		return NETDATA();
	}

	RxData = (NETDATA*)buffer;

	return *RxData;
}

void Engine::CNetwork::Free(void)
{
	// ���� ����
	closesocket(socket_client);

	Safe_Release(m_pGraphicDev);
}
