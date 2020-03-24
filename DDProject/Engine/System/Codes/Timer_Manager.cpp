#include "Timer_Manager.h"
#include "Timer.h"

USING(Engine)

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}


CTimer_Manager::~CTimer_Manager()
{
}

float CTimer_Manager::Get_TimeDelta(const TCHAR * pTimerTag) const
{
	CTimer* pTimer_Find = Find_Timer(pTimerTag);
	
	if (nullptr == pTimer_Find)
		return 0.0f;

	return pTimer_Find->Get_TimeDelta();	
}

HRESULT CTimer_Manager::Ready_Timer(const TCHAR * pTimerTag, CTimer * pTimer)
{
	CTimer* pTimer_Find = Find_Timer(pTimerTag);

	if (nullptr != pTimer_Find)
		return E_FAIL;

	m_mapTimer.insert(MAPTIMER::value_type(pTimerTag, pTimer));		

	return NOERROR;
}

void CTimer_Manager::Compute_TimeDelta(const TCHAR * pTimerTag)
{
	CTimer* pTimer_Find = Find_Timer(pTimerTag);

	if (nullptr == pTimer_Find)
		return;

	pTimer_Find->Compute_TimeDelta();

}

CTimer * CTimer_Manager::Find_Timer(const TCHAR * pTimerTag) const
{
	auto iter = m_mapTimer.find(pTimerTag);

	if(iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::Free(void)
{
	/*for_each(m_mapTimer.begin(), m_mapTimer.end(), Engine::CRelease_Pair());*/

	unsigned long dwRefCnt = 0;

	for (auto& Pair : m_mapTimer)
	{
		dwRefCnt = Safe_Release(Pair.second);	
	}

	m_mapTimer.clear();
}
