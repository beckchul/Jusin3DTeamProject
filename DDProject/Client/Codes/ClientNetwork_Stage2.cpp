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
	// Scene 세팅
	m_eCurrentScene = eScene;

	// 플레이어 생성 요청
	Engine::NETDATA tData;
	ZeroMemory(&tData, sizeof(Engine::NETDATA));
	tData.chKey = 72;

	tData.eMessage = Engine::NETMSG_CLIENT_CREATE_PLAYER;
	SendMsg(tData);

	// Chat 생성
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

		if (72 != RxData.chKey) // 제대로 된 값이 들어왔는지 확인하는것.
		{
			std::cout << "잘못된 키(72가 아닌) 들어옴" << std::endl;

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
				cout << "잘못된 서버 인덱스 초기화" << endl; // 가끔 Message가 망가져서 들어온다.
				break;
			}

			g_iClientIndex = RxData.iIndex;		// 서버 접속시 한번만 실행
			std::cout << "클라이언트 번호 : " << g_iClientIndex << std::endl;

			m_bClientSetting[g_iClientIndex] = true;
			break;

		case Engine::NETMSG_SERVER_CREATE_PLAYER: // 플레이어 생성
			Message_Create_Player(RxData);
			break;

		case Engine::NETMSG_SERVER_DELETE_PLAYER: // 플레이어 삭제
			Message_Delete_Player(RxData.iIndex);
			break;

		case Engine::NETMSG_SERVER_PLAYER_UPDATE: // 플레이어 업데이트(위치)
			Message_Update_Player(RxData);
			break;

		case Engine::NETMSG_SERVER_SEND_CHATTING: // 채팅
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
		// printf("정상\n");
		break;

	case WSAENOTCONN:	// 소켓이 연결되어 있지 않을 때
		printf("-- 1번 에러\n");
		break;

	case WSAESHUTDOWN:	// 소켓이 종료되었을 때
		printf("-- 2번 에러\n");
		break;

	case WSANOTINITIALISED:
		// 성공적인 WSAStartup 함수의 호출은이 기능을 사용하기 전에 발생해야합니다.
		printf("-- 3번 에러\n");
		break;

	case WSAENETDOWN:
		// 네트워크 서브 시스템에 오류가 발생했습니다.
		printf("-- 4번 에러\n");
		break;

	case WSAEFAULT:
		// 버피 파라미터는 완전히 사용자 주소 공간의 유효 부분에 포함되지 않는다.
		printf("-- 5번 에러\n");
		break;

	case WSAEINTR:
		// 차단)호출을 취소했습니다 WSACancelBlockingCall .
		printf("-- 6번 에러\n");
		break;

	case WSAEINPROGRESS:
		// 블럭킹 윈속 v1.1이 현재 진행 중이거나, 서비스 프로 바이더가 콜백 함수를 여전히 처리하고 있습니다.
		printf("-- 7번 에러\n");
		break;

	case WSAENETRESET:
		// 연결 지향 소켓의 경우, 
		// 이 오류는 연결이로 인해 파괴되었음을 나타냅니다.
		// 연결 유지 작업이 진행되는 동안 오류가 감지 활동.
		// 데이터 그램 소켓의 경우, 이 오류가 살 수있는 
		// 기간이 만료되었음을 나타냅니다.
		printf("-- 8번 에러\n");
		break;

	case WSAENOTSOCK:
		// 기술자는 소켓이 아니다.
		printf("-- 9번 에러\n");
		break;

	case WSAEOPNOTSUPP:
		// MSG_OOB가 지정 되었으나, 소켓 타입 SOCK_STREAM과 같은 
		// 스트림 스타일 아닌, OOB 데이터는이 소켓과 관련된 
		// 통신 도메인에서 지원되지 않습니다, 
		// 소켓이 단방향 만 작업을 보내 지원합니다.
		printf("-- 10번 에러\n");
		break;
	case WSAEWOULDBLOCK:
		// 소켓은 블로킹으로 표시되며, 
		// 수신 동작을 차단하는 것입니다.
		//printf(" - No Message\n");
		//printf("-- 11번 에러 : \n");
		break;

	case WSAEMSGSIZE:
		// 이 메시지는 지정된 버퍼에 비해 너무 크며 잘 렸습니다.
		printf("-- 11번 에러\n");
		break;

	case WSAEINVAL:
		// 소켓에 연결되지 않은 결합 , 또는 알 수없는 플래그가
		// 지정 된, 또는 MSG_OOB가 SO_OOBINLINE 사용하거나 
		// 사용과 소켓 지정 (전용 바이트 스트림 소켓에 대한) 
		// LEN이 제로 또는 부의이었다.
		printf("-- 12번 에러\n");
		break;

	case WSAECONNABORTED:
		// 가상 회로 인해 타임 아웃 또는 다른 고장을 종결시켰다.
		// 더 이상 사용할 수 같은 응용 프로그램은 소켓을 닫습니다 없습니다.
		printf("-- 13번 에러\n");
		break;

	case WSAETIMEDOUT:
		// 연결은 네트워크 장애로 인해 또는 
		// 피어 시스템이 응답하지 못했기 때문에 삭제되었습니다.
		printf("-- 14번 에러\n");
		break;

	case WSAECONNRESET:
		// 가상 회선은 경질 또는 불완전 닫기를 실행하는 
		// 원격 측에 의해 리셋되었다.
		// 더 이상 사용할 수 같은 응용 프로그램은 소켓을 닫습니다 없습니다.
		// 은 UDP 데이터 그램 소켓에서이 오류는 
		// 이전의 전송 작업은 ICMP "포트에 연결할 수 없음"메시지가 
		// 결과 것을 나타냅니다.
		printf("원격에서 연결을 끊었습니다.\n");
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
		if (false == m_bClientSetting[i]) // 서버로부터 생성하라는 메세지를 받았는지
			continue;

		if (nullptr == m_pPlayerArray[i]) // 아직 안만들었다면 만들자
			Create_Player(i);
	}
}

void CClientNetwork_Stage2::Check_Player_Delete(void)
{
	for (int i = 1; i < MAX_CLIENT_NUM2; ++i)
	{
		if (true == m_bClientSetting[i]) // 서버로부터 삭제하라는 메세지를 받았는지
			continue;

		if (nullptr != m_pPlayerArray[i]) // 아직 삭제 안했다면 삭제
			Delete_Player(i);
	}
}

void CClientNetwork_Stage2::Create_Player(int iIndex)
{
	++m_iPlayerNum;

	cout << g_iClientIndex << " 번 클라 : " << iIndex << " 번 플레이어 생성 " << endl;

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

	cout << iIndex << "번 클라이언트 탈주 " << endl;

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
