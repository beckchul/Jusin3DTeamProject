#include "Management.h"
#include "Scene.h"
#include "Renderer.h"
#include "Component_Manager.h"
#include "Light_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

Engine::CManagement::CManagement(void)
: m_pCurrentScene(nullptr)
, m_pRenderer(nullptr)
, m_bAfterFirstRender(false)
{

}

Engine::CManagement::~CManagement(void)
{

}

HRESULT Engine::CManagement::Ready_Management(const _uint& uIndex, const _tchar* pRendererTag)
{
	m_pRenderer = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(uIndex, pRendererTag);
	if(nullptr == m_pRenderer)
		return E_FAIL;

	return S_OK;
}

_int Engine::CManagement::Update_Management(const _float& fTimeDelta)
{
	if(nullptr == m_pCurrentScene)
		return -1;	

	return m_pCurrentScene->Update_Scene(fTimeDelta);
}

void Engine::CManagement::Render_Management(void)
{
	if (nullptr == m_pRenderer)
		return;

	m_pRenderer->Render_GameObject();

	if (nullptr == m_pCurrentScene)
		return;

	if (FALSE == m_bAfterFirstRender)
	{
		m_bAfterFirstRender = true;
		m_pCurrentScene->FirstRender_Finish();
	}

	m_pCurrentScene->Render_Scene();
		
}

HRESULT Engine::CManagement::SetUp_CurrentScene(CScene* pCurrentScene)
{
	if(nullptr == pCurrentScene)
		return E_FAIL;

	if(nullptr != m_pCurrentScene)
	{
		if(0 != Engine::Safe_Release(m_pCurrentScene))
			MSG_BOX(L"Current Scene Release Failed");
	}

	m_pCurrentScene = pCurrentScene;

	m_bAfterFirstRender = false;

	return S_OK;
}

void Engine::CManagement::Free(void)
{
	Engine::Safe_Release(m_pRenderer);

	Engine::Safe_Release(m_pCurrentScene);	
}

