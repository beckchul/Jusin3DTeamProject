#include "InfoSubject.h"

IMPLEMENT_SINGLETON(Engine::CInfoSubject)

Engine::CInfoSubject::CInfoSubject(void)
{
}

Engine::CInfoSubject::~CInfoSubject(void)
{
}

void Engine::CInfoSubject::Add_Data(int message, void* pData)
{
	if (pData) // nullptr인 데이타도 받게 바꿀수도 있다
	{
		MAPDATALIST::iterator	iter = m_mapDatalist.find(message);
		if (iter == m_mapDatalist.end())
		{
			m_mapDatalist[message] = DATALIST();
		}
		m_mapDatalist[message].push_back(pData);
		Notify_Message(message);
	}
}

void Engine::CInfoSubject::Add_DataFront(int message, void * pData)
{
	if (pData)
	{
		MAPDATALIST::iterator	iter = m_mapDatalist.find(message);
		if (iter == m_mapDatalist.end())
		{
			m_mapDatalist[message] = DATALIST();
		}
		m_mapDatalist[message].push_front(pData);
		Notify_Message(message);
	}
}

void Engine::CInfoSubject::Remove_Data(int message, void* pData)
{
	MAPDATALIST::iterator		mapiter = m_mapDatalist.find(message);
	if (mapiter != m_mapDatalist.end())
	{
		DATALIST::iterator		iterlist = mapiter->second.begin();
		DATALIST::iterator		iterlist_end = mapiter->second.end();

		for (; iterlist != iterlist_end; ++iterlist)
		{
			if ((*iterlist) == pData)
			{
				mapiter->second.erase(iterlist);
				return;
			}
		}
	}
}

void Engine::CInfoSubject::Free(void)
{
	CSubject::Free();

	for (MAPDATALIST::iterator iter = m_mapDatalist.begin();
		iter != m_mapDatalist.end(); ++iter)
	{
		iter->second.clear();
	}
	m_mapDatalist.clear();
}

list<void*>* Engine::CInfoSubject::GetDatalist(int message)
{
	MAPDATALIST::iterator	iter = m_mapDatalist.find(message);
	if (iter == m_mapDatalist.end())
		return NULL;

	return &iter->second;
}