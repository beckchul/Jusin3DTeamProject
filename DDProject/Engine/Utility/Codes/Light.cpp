#include "Light.h"
#include "View_Texture.h"
#include "Component_Manager.h"
#include "DataManager.h"

USING(Engine)

Engine::CLight::CLight(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pBufferCom(nullptr)
{
	m_pGraphicDev->AddRef();
}

Engine::CLight::~CLight(void)
{

}

HRESULT Engine::CLight::Ready_Light(const D3DLIGHT9* pLightInfo)
{
	m_LightInfo = *pLightInfo;

	D3DVIEWPORT9		Viewport;
	m_pGraphicDev->GetViewport(&Viewport);

	m_pBufferCom = CView_Texture::Create(m_pGraphicDev, &_vec4(0.f, 0.f, (float)Viewport.Width, (float)Viewport.Height));
	if (nullptr == m_pBufferCom)
		return E_FAIL;

	return S_OK;
}

void Engine::CLight::Render_Light(LPD3DXEFFECT pEffect)
{
	_uint		iPassIdx = 0;

	if (D3DLIGHT_DIRECTIONAL == m_LightInfo.Type)
	{
		iPassIdx = 0;
		_vec4		vDir = _vec4(m_LightInfo.Direction, 0.f);
		pEffect->SetVector("g_LightDir", D3DXVec4Normalize(&vDir, &vDir));
	}

	else if (D3DLIGHT_POINT == m_LightInfo.Type)
	{
		iPassIdx = 1;
		pEffect->SetVector("g_LightPosition", &_vec4(m_LightInfo.Position, 1.f));
		pEffect->SetFloat("g_fRange", m_LightInfo.Range);
	}
	pEffect->SetFloat("g_BrightNess", m_LightInfo.Diffuse.a);

	pEffect->SetVector("g_LightDiffuse", (_vec4*)&m_LightInfo.Diffuse);
	pEffect->SetVector("g_LightAmbient", (_vec4*)&m_LightInfo.Ambient);
	pEffect->SetVector("g_LightSpecular", (_vec4*)&m_LightInfo.Specular);

	_matrix		matView, matProj, matViewProjInv;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	
	D3DXMatrixMultiply(&matViewProjInv, &matView, &matProj);
	D3DXMatrixInverse(&matViewProjInv, nullptr, &matViewProjInv);

	pEffect->SetVector("g_CamPosition", (_vec4*)&CDataManager::GetInstance()->Get_CamPoistion());
	pEffect->SetMatrix("g_matViewProjInv", &matViewProjInv);
	
	pEffect->CommitChanges();

	pEffect->BeginPass(iPassIdx);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
}

CLight* Engine::CLight::Create(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo)
{
	CLight* pInstance = new CLight(pGraphicDev);

	if (FAILED(pInstance->Ready_Light(pLightInfo)))
	{
		MSG_BOX(L"CLight Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;

}
void Engine::CLight::Free(void)
{
	Engine::Safe_Release(m_pGraphicDev);
	Engine::Safe_Release(m_pBufferCom);
}

