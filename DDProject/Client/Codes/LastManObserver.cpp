#include "stdafx.h"
#include "LastManObserver.h"
#include "InfoSubject.h"
#include "Camera_Manager.h"

CLastManObserver::CLastManObserver()
{
}

CLastManObserver::~CLastManObserver()
{
}

void CLastManObserver::Update_Observer(int message)
{
	list<void*>* pDatalist = Engine::CInfoSubject::GetInstance()->GetDatalist(message);

	if (nullptr == pDatalist)
		return;

	switch (message)
	{
	case MESSAGE_LASTMAN_INFO:
	{
		list<void*>::iterator iter_begin = pDatalist->begin();

		for (; iter_begin != pDatalist->end(); ++iter_begin)
		{
			LASTMAN_INFO* pInfo = (LASTMAN_INFO*)(*iter_begin);
			if (nullptr == pInfo)
				continue;

			m_arrLastManInfo[pInfo->iPlayerIdx] = pInfo; // 해당 플레이어의 Info를 갱신
		}
		break;
	}

	case MESSAGE_LASTMAN_DELETE:
	{
		int iIndex = *(int*)pDatalist->back();
		m_arrLastManInfo[iIndex] = nullptr;

		--m_iMaxPlayerCount;
		break;
	}
	case MESSAGE_LASTMAN_COMPUTER_ADD:
	{
		++m_iAICount;
		++m_iMaxAICount;
		break;
	}
	case MESSAGE_LASTMAN_COMPUTER_DEAD:
	{
		--m_iAICount;
		break;
	}
	case MESSAGE_LASTMAN_PLAYER_ADD:
	{
		++m_iPlayerCount;
		++m_iMaxPlayerCount;
		break;
	}
	case MESSAGE_LASTMAN_PLAYER_DEAD:
	{
		--m_iPlayerCount;

		if ( ( g_eLastMan_Progress == LASTMAN_ROUND_START ||
			g_eLastMan_Progress == LASTMAN_ROUND_ELECTRIC_READY ||
			g_eLastMan_Progress == LASTMAN_ROUND_ELECTRIC_ACTIVATE)
			&& m_iPlayerCount == 1)
		{
			cout << "승리" << endl;

			g_eLastMan_Progress = LASTMAN_ROUND_FINISH;

			_uint uWinner = Find_Winner();
			if (uWinner != -1)
			{
				g_iClientIndex = uWinner;
				CCamera_Manager::GetInstance()->Change_Camera(CAMERA_GOBLIN, 0, 1.f, true);
			}
		}
		break;
	}
	default:
		break;
	}
}

HRESULT CLastManObserver::Ready_Observer(void)
{
	ZeroMemory(m_arrLastManInfo, sizeof(m_arrLastManInfo));

	return NOERROR;
}

_uint CLastManObserver::Find_Winner(void)
{
	for (int i = 0; i < MAX_CLIENT_NUM2; ++i)
	{
		if (nullptr == m_arrLastManInfo[i])
			continue;

		if (TRUE == m_arrLastManInfo[i]->tUnitInfo.bDead)
			continue;

		return i;
	}

	return -1;
}

CLastManObserver* CLastManObserver::Create(void)
{
	CLastManObserver*		pInstance = new CLastManObserver;

	if (FAILED(pInstance->Ready_Observer()))
	{
		MSG_BOX(L"CLastManObserver Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void CLastManObserver::Free(void)
{
}
