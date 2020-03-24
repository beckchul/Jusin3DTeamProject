#pragma once
#ifndef Timer_Manager_h__
#define Timer_Manager_h__

#include "Timer.h"

BEGIN(Engine)
class ENGINE_DLL CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)
public:
	explicit CTimer_Manager(void);
	virtual ~CTimer_Manager(void);

public:	// Getter
	float Get_TimeDelta(const TCHAR* pTimerTag) const;

public:	// Function
	HRESULT Ready_Timer(const TCHAR* pTimerTag, CTimer* pTimer);
	void Compute_TimeDelta(const TCHAR* pTimerTag);

private: // Member Variable 
	unordered_map<const TCHAR*, CTimer*>			m_mapTimer;
	typedef unordered_map<const TCHAR*, CTimer*>	MAPTIMER;

private:
	CTimer* Find_Timer(const TCHAR* pTimerTag) const;

private:
	virtual void Free(void) override;
};

END

#endif