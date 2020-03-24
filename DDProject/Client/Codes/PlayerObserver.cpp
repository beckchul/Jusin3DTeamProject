#include "stdafx.h"
#include "PlayerObserver.h"
#include "InfoSubject.h"

CPlayerObserver::CPlayerObserver()
{
}

CPlayerObserver::~CPlayerObserver()
{
}

void CPlayerObserver::Update_Observer(int message)
{
	list<void*>* pDatalist = Engine::CInfoSubject::GetInstance()->GetDatalist(message);

	if (nullptr == pDatalist)
		return;

	switch (message)
	{
	case MESSAGE_PLAYER_INFO:
	{
		list<void*>::iterator iter_begin = pDatalist->begin();

		for (; iter_begin != pDatalist->end(); ++iter_begin)
		{
			PLAYER_INFO* pInfo = (PLAYER_INFO*)(*iter_begin);
			if (nullptr == pInfo)
				continue;

			m_arrPlayerInfo[pInfo->iPlayerIdx] = pInfo; // 해당 플레이어의 Info를 갱신
		}
		
		break;
	}

	case MESSAGE_PLAYER_DELETE:
	{
		int iIndex = *(int*)pDatalist->back();
		m_arrPlayerInfo[iIndex] = nullptr;
		break;
	}

	case MESSAGE_PLAYER_MP:
	{
		pair<int, int> pairMp = *(pair<int, int>*)pDatalist->back();

		if (nullptr == m_arrPlayerInfo[pairMp.first])
			break;

		int iMaxMp = m_arrPlayerInfo[pairMp.first]->tUnitInfo.iMpMax;
		if(pairMp.second >= iMaxMp)
			const_cast<PLAYER_INFO*>(m_arrPlayerInfo[pairMp.first])->tUnitInfo.iMp = iMaxMp;
		else
			const_cast<PLAYER_INFO*>(m_arrPlayerInfo[pairMp.first])->tUnitInfo.iMp = pairMp.second;
		break;
	}

	default:
		break;
	}
}

HRESULT CPlayerObserver::Ready_Observer(void)
{
	ZeroMemory(m_arrPlayerInfo, sizeof(m_arrPlayerInfo));

	return NOERROR;
}

CPlayerObserver* CPlayerObserver::Create(void)
{
	CPlayerObserver*		pInstance = new CPlayerObserver;

	if (FAILED(pInstance->Ready_Observer()))
	{
		MSG_BOX(L"CPlayerObserver Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerObserver::Free(void)
{
}
