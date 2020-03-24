#ifndef LastManObserver_h__
#define LastManObserver_h__

#include "Observer.h"
#include "Defines.h"
#include "Engine_Defines.h"

namespace Engine
{
	class CTransform;
}

class CLastManObserver : public CObserver
{
private:
	explicit CLastManObserver(void);
	virtual ~CLastManObserver(void);

public:
	// Getter
	const LASTMAN_INFO* Get_PlayerInfo(int iPlayerIdx)
	{
		return m_arrLastManInfo[iPlayerIdx];
	}
	const _int Get_ComputerCount(void)
	{
		return m_iAICount;
	}
	const _int Get_MaxComputerCount(void)
	{
		return m_iMaxAICount;
	}
	const _int Get_PlayerCount(void)
	{
		return m_iPlayerCount;
	}
	const _int Get_MaxPlayerCount(void)
	{
		return m_iMaxPlayerCount;
	}
public:
	virtual void Update_Observer(int message);
	virtual HRESULT Ready_Observer(void);

	_uint Find_Winner(void);

private:
	const LASTMAN_INFO*			m_arrLastManInfo[MAX_CLIENT_NUM2];
	_int						m_iAICount = 0;
	_int						m_iMaxAICount = 0;
	_int						m_iPlayerCount = 0;
	_int						m_iMaxPlayerCount = 0;
public:
	static CLastManObserver* Create(void);
private:
	virtual void Free(void);
};

#endif//LastManObserver_h__
