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

	// 서버로 부터 메세지를 받은 이후의 함수들
	virtual void RecvData(void);

	void Message_Create_Player(Engine::NETDATA tData);
	void Message_Delete_Player(int iIndex);
	void Message_Update_Player(Engine::NETDATA tData);

	void Message_Chat(Engine::NETDATA tData);

	void ErrorCheck(void);

	// 그 외
	void Check_Player_Create(void); // 서버로부터 플레이어를 생성하라는 메세지를 받았다면 플레이어를 생성해주자
	void Check_Player_Delete(void); // 서버로부터 플레이어를 삭제하라는 메세지를 받았다면

	void Create_Player(int iIndex); // 실제로 플레이어를 생성해서 배열에 저장
	void Delete_Player(int iIndex); // 실제로 플레이어를 삭제

	void Stage_Clear(void); // 스테이지 넘어갈 때

private:
	bool					m_bClientSetting[MAX_CLIENT_NUM2]; // 서버에게 플레이어 생성하라는 메세지를 받았는지
	Engine::CGameObject*	m_pPlayerArray[MAX_CLIENT_NUM2]; // 플레이어 배열

	_int					m_iIndex = -1;

	Engine::CGameObject*	m_pChat = nullptr; // 채팅

	SCENEID		m_eCurrentScene = SCENE_END; // 현재 Scene
private:
	virtual void Free(void);
};

#endif