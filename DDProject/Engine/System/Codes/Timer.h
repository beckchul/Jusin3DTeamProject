#pragma once
#ifndef Timer_h__
#define Timer_h__

#include "Engine_Defines.h"

BEGIN(Engine)
// TimeDelta라는 녀석을 생성하고 값을 갱신토록한다.
class ENGINE_DLL CTimer : public CBase
{
private:
	explicit CTimer(void);
	virtual ~CTimer(void);
public: // Getter
	float Get_TimeDelta(void) const { return m_fTimeDelta; }

public:
	HRESULT Ready_Timer(void);
	void Compute_TimeDelta(void);

private:
	LARGE_INTEGER		m_FrameTime;
	LARGE_INTEGER		m_LastTime;
	LARGE_INTEGER		m_FixTime;
	LARGE_INTEGER		m_CpuTick;

private:
	float				m_fTimeDelta;

public:
	static CTimer* Create(void);

private:
	virtual void Free(void);
};

END

#endif