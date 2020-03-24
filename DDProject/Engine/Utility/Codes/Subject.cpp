#include "Subject.h"

#include "Observer.h"

Engine::CSubject::CSubject(void)
{
}

Engine::CSubject::~CSubject(void)
{
}

void Engine::CSubject::Subscribe_Observer(CObserver* pObserver)
{
	if (nullptr == pObserver)
		return;

	m_Observerlist.push_back(pObserver);
}

void Engine::CSubject::UnSubscribe_Observer(CObserver* pObserver)
{
	OBSERVERLIST::iterator	iter = m_Observerlist.begin();
	OBSERVERLIST::iterator	iter_end = m_Observerlist.end();

	for (; iter != iter_end; )
	{
		if ((*iter) == pObserver)
		{
			m_Observerlist.erase(iter);
			return;
		}
		else
			++iter;
	}
}

void Engine::CSubject::Notify_Message(int message)
{
	for (auto iter : m_Observerlist)
	{
		iter->Update_Observer(message);
	}
}

void Engine::CSubject::Free(void)
{
	m_Observerlist.clear();
}

