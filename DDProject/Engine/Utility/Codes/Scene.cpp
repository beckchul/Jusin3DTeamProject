#include "Scene.h"

USING(Engine)

CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pComponentMgr(Engine::CComponent_Manager::GetInstance())
	, m_pObjectMgr(Engine::CObject_Manager::GetInstance())
{
	m_pGraphicDev->AddRef();
	m_pComponentMgr->Add_Ref();
	m_pObjectMgr->Add_Ref();
}

CScene::~CScene(void)
{

}

int Engine::CScene::Update_Scene(const float& fTimeDelta)
{
	if (nullptr == m_pObjectMgr)
		return -1;	

	return m_pObjectMgr->Update_ObjMgr(fTimeDelta);	
}

void CScene::Render_Scene(void)
{

}

void CScene::Free(void)
{
	DWORD dwRefCnt = 0;
	
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pComponentMgr);
	Safe_Release(m_pGraphicDev);
}