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

	// 서버로 부터 메세지를 받은 이후의 함수들
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
	void Check_Player_Create(void); // 서버로부터 플레이어를 생성하라는 메세지를 받았다면 플레이어를 생성해주자
	void Check_Player_Delete(void); // 서버로부터 플레이어를 삭제하라는 메세지를 받았다면

	void Create_Player(int iIndex); // 실제로 플레이어를 생성해서 배열에 저장
	void Delete_Player(int iIndex); // 실제로 플레이어를 삭제

	void Stage_Clear(void); // 스테이지 넘어갈 때

	void Send_ChangeMessage(_int iBeforePlayer, _int iAfterPlayer);
	void Change_Player(_int iBeforePlayer, _int iAfterPlayer); // 플레이어 교체

	// Stage2
	void Message_Update_Player2(Engine::NETDATA tData);
	void Message_Decal(Engine::NETDATA tData);
	void Message_Kill(Engine::NETDATA tData);

	// 그 외
	void Check_Player_Create2(void); // 서버로부터 플레이어를 생성하라는 메세지를 받았다면 플레이어를 생성해주자
	void Check_Player_Delete2(void); // 서버로부터 플레이어를 삭제하라는 메세지를 받았다면

	void Create_Player2(int iIndex); // 실제로 플레이어를 생성해서 배열에 저장
	void Delete_Player2(int iIndex); // 실제로 플레이어를 삭제

	void Stage_Clear2(void); // 스테이지 넘어갈 때
	void Message_ID(Engine::NETDATA tData);
	void Stage2_State(Engine::NETDATA tData);

	void Create_Stage2Monster(void);

	void Reset_Round(void);

private:
	// Stage1
	_bool					m_bClientSetting[MAX_CLIENT_NUM]; // 서버에게 플레이어 생성하라는 메세지를 받았는지
	Engine::CGameObject*	m_pPlayerArray[MAX_CLIENT_NUM]; // 플레이어 배열

	SCENEID		m_eCurrentScene = SCENE_END; // 현재 Scene
	PHASE_TYPE  m_ePhaseType = BUILD_PHASE;

private:
	// Stage2
	_bool							m_bClientSetting2[MAX_CLIENT_NUM2]; // 서버에게 플레이어 생성하라는 메세지를 받았는지
	_bool							m_bClientIDSetting[MAX_CLIENT_NUM2]; // ID를 등록했는지
	Engine::CGameObject*			m_pPlayerArray2[MAX_CLIENT_NUM2]; // 라스트맨 배열
	vector<Engine::CGameObject*>	m_vecAI; // AI들 보관			 

private:
	_int					m_iIndex = -1;
	Engine::CGameObject*	m_pChat = nullptr; // 채팅
private:
	virtual void Free(void);
};

#endif