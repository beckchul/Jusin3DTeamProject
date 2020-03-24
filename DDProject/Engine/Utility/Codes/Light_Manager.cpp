#include "Light_Manager.h"
#include "Light.h"

USING(Engine)
IMPLEMENT_SINGLETON(CLight_Manager);

Engine::CLight_Manager::CLight_Manager(void)
{

}

Engine::CLight_Manager::~CLight_Manager(void)
{

}

HRESULT Engine::CLight_Manager::Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo)
{
	CLight*		pLight = CLight::Create(pGraphicDev, pLightInfo);

	if(nullptr == pLight)
		return 0;

	m_LightList.push_back(pLight);

	return S_OK;
}

void Engine::CLight_Manager::Render_Light(LPD3DXEFFECT pEffect)
{
	LIGHTLIST::iterator	iter = m_LightList.begin();
	LIGHTLIST::iterator	iter_end = m_LightList.end();

	for( ; iter != iter_end; ++iter)
	{
		(*iter)->Render_Light(pEffect);
	}
}

void CLight_Manager::Reset_Light(void)
{
	for_each(m_LightList.begin(), m_LightList.end(), CRelease_Single());

	m_LightList.clear();
}

void Engine::CLight_Manager::Free(void)
{
	for_each(m_LightList.begin(), m_LightList.end(), CRelease_Single());

	m_LightList.clear();
}

