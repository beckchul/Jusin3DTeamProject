#include "Timer.h"

USING(Engine)

CTimer::CTimer()
{
}


CTimer::~CTimer()
{
}

//  타이머가 생성될대 한번호출
HRESULT CTimer::Ready_Timer(void)
{
	QueryPerformanceCounter(&m_FrameTime); 
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceFrequency(&m_CpuTick);	

	return NOERROR;
}

// 매 프레임마다 호출된다.
void CTimer::Compute_TimeDelta(void)
{
	QueryPerformanceCounter(&m_FrameTime);

	m_fTimeDelta = float((m_FrameTime.QuadPart - m_LastTime.QuadPart)) / float(m_CpuTick.QuadPart);

	m_LastTime = m_FrameTime;

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_FixTime = m_FrameTime;
	}
}

CTimer * CTimer::Create(void)
{
	CTimer *	pInstance = new CTimer();

	if (FAILED(pInstance->Ready_Timer()))
	{
		MSG_BOX(L"CTimer Created Failed");
		Safe_Release(pInstance);		
	}	
	return pInstance;
}

void CTimer::Free(void)
{
}
