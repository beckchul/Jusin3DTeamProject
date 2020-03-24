#include "Material.h"

USING(Engine)

Engine::CMaterial::CMaterial(LPDIRECT3DDEVICE9 pGraphicDev)
: m_pGraphicDev(pGraphicDev)
{
	ZeroMemory(&m_MtrlInfo, sizeof(D3DMATERIAL9));

	m_pGraphicDev->AddRef();
}

Engine::CMaterial::CMaterial(const CMaterial& rhs)
: m_pGraphicDev(rhs.m_pGraphicDev)
{
	memcpy(&m_MtrlInfo, &rhs.m_MtrlInfo, sizeof(D3DMATERIAL9));
	m_pGraphicDev->AddRef();
}

Engine::CMaterial::~CMaterial(void)
{

}

void Engine::CMaterial::Change_Material(MATERIAL eType, void* pValue)
{
	if(MTRL_POWER == eType)	
		*((_float*)((D3DCOLORVALUE*)&m_MtrlInfo + eType)) = *((_float*)pValue);
	else
		*((D3DCOLORVALUE*)&m_MtrlInfo + eType) = *((D3DCOLORVALUE*)pValue);
}

HRESULT Engine::CMaterial::Ready_Component(D3DXCOLOR Diffuse, D3DXCOLOR Ambient, D3DXCOLOR Emissive, D3DXCOLOR Specular, const _float& fPower)
{
	m_MtrlInfo.Diffuse = Diffuse;
	m_MtrlInfo.Ambient = Ambient;
	m_MtrlInfo.Specular = Specular;
	m_MtrlInfo.Emissive = Emissive;
	m_MtrlInfo.Power = fPower;

	return S_OK;
}

CComponent* Engine::CMaterial::Clone(void)
{
	return new CMaterial(*this);
}

CMaterial* Engine::CMaterial::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR Diffuse, D3DXCOLOR Ambient, D3DXCOLOR Emissive, D3DXCOLOR Specular, const _float& fPower)
{
	CMaterial*		pInstance = new CMaterial(pGraphicDev);

	if(FAILED(pInstance->Ready_Component(Diffuse, Ambient, Emissive, Specular, fPower)))
	{
		MSG_BOX(L"CMaterial Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::CMaterial::Free(void)
{
	Engine::Safe_Release(m_pGraphicDev);
}

