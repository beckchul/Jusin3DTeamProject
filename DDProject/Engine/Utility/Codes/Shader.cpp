#include "Shader.h"

USING(Engine)

Engine::CShader::CShader(LPDIRECT3DDEVICE9 pGraphicDev)
: m_pGraphicDev(pGraphicDev)
, m_pEffect(nullptr)
, m_pErrBuffer(nullptr)
{
	m_pGraphicDev->AddRef();
}

Engine::CShader::CShader(const CShader& rhs)
: m_pGraphicDev(rhs.m_pGraphicDev)
, m_pEffect(rhs.m_pEffect)
, m_pErrBuffer(rhs.m_pErrBuffer)
{
	m_pGraphicDev->AddRef();
	m_pEffect->AddRef();

	if(nullptr != m_pErrBuffer)
		m_pErrBuffer->AddRef();
}

Engine::CShader::~CShader(void)
{

}

HRESULT Engine::CShader::Ready_Shader(const _tchar* pFilePath)
{
	// hlsl로 제작한 파일을 번역하여 LPD3DXEFFCT객체에 보관한다.
	if(FAILED(D3DXCreateEffectFromFile(m_pGraphicDev, pFilePath, nullptr, nullptr, D3DXSHADER_DEBUG
		, nullptr, &m_pEffect, &m_pErrBuffer)))
	{
		// 내가 짠 코드에 에러가 있을 경우
		MessageBoxA(nullptr, (char*)m_pErrBuffer->GetBufferPointer(), nullptr, MB_OK);

		return E_FAIL;		
	}

	if(nullptr != m_pErrBuffer)
	{
		MessageBoxA(nullptr, (char*)m_pErrBuffer->GetBufferPointer(), nullptr, MB_OK);
	}

/*
	m_pEffect->SetMatrix();
	m_pEffect->SetFloat();
	m_pEffect->SetVector();
	m_pEffect->SetTexture();*/

	return S_OK;
}

CComponent* Engine::CShader::Clone(void)
{
	return new CShader(*this);
}

CShader* Engine::CShader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath)
{
	CShader*		pInstance = new CShader(pGraphicDev);

	if(FAILED(pInstance->Ready_Shader(pFilePath)))
	{
		MSG_BOX(L"CShader Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::CShader::Free(void)
{
	Engine::Safe_Release(m_pEffect);
	Engine::Safe_Release(m_pErrBuffer);
	Engine::Safe_Release(m_pGraphicDev);
}

