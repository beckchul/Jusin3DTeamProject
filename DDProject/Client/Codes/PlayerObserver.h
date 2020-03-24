#ifndef PlayerObserver_h__
#define PlayerObserver_h__

#include "Observer.h"
#include "Defines.h"
#include "Engine_Defines.h"

namespace Engine
{
	class CTransform;
}

class CPlayerObserver : public CObserver
{
private:
	explicit CPlayerObserver(void);
	virtual ~CPlayerObserver(void);

public:
	// Getter
	const PLAYER_INFO* Get_PlayerInfo(int iPlayerIdx)
	{
		return m_arrPlayerInfo[iPlayerIdx];
	}
public:
	virtual void Update_Observer(int message);
	virtual HRESULT Ready_Observer(void);
	
private:
	const PLAYER_INFO*			m_arrPlayerInfo[MAX_CLIENT_NUM];
public:
	static CPlayerObserver* Create(void);
private:
	virtual void Free(void);
};

#endif//PlayerObserver_h__
