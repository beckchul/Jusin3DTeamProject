#include "stdafx.h"
#include "SceneObserver.h"

#include "InfoSubject.h"
#include "Object_Manager.h"

CSceneObserver::CSceneObserver(void)
	:m_eCurrentScene(SCENE_END)
{
}

CSceneObserver::~CSceneObserver(void)
{
}

void CSceneObserver::Update_Observer(int message)
{
	const list<void*>* pDatalist = Engine::CInfoSubject::GetInstance()->GetDatalist(message);

	if (nullptr == pDatalist)
		return;

	switch (message)
	{
	case MESSAGE_SCENECHANGE:
		m_eCurrentScene = *(SCENEID*)pDatalist->back();
		break;

	default:
		break;
	}
}

HRESULT CSceneObserver::Ready_Observer(void)
{
	return NOERROR;
}

CSceneObserver* CSceneObserver::Create(void)
{
	CSceneObserver*		pInstance = new CSceneObserver;

	if (FAILED(pInstance->Ready_Observer()))
	{
		MSG_BOX(L"CSceneObserver Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void CSceneObserver::Free(void)
{
}

