#include "Target.h"
#include "View_Texture.h"

USING(Engine)

CTarget::CTarget(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pTargetTexture(nullptr)
	, m_pOldSurface(nullptr)
	, m_pRenderTarget(nullptr)
	, m_pBufferCom(nullptr)
{
	m_pGraphicDev->AddRef();
}


CTarget::~CTarget()
{
}

HRESULT CTarget::Ready_Target(const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color, bool bShadowMap)
{
	// ����Ÿ������ �������� �ؽ��ĸ� ������ �����Ѵ�.
	if (FAILED(D3DXCreateTexture(m_pGraphicDev, iSizeX, iSizeY, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, &m_pTargetTexture)))
		return E_FAIL;

	// �ؽ��ľȿ� �����Ǿ��ִ� Surface�� ���´�.
	if (FAILED(m_pTargetTexture->GetSurfaceLevel(0, &m_pRenderTarget)))
		return E_FAIL;

	m_Color = Color;

	m_pGraphicDev->GetRenderTarget(0, &m_pBackBuff);

	// ��������� Ÿ���� �׸��ڸ��̸� �ϵ��� ��ϵ� ���̹��۸� �̸������ϰ� ���̹��۸� �����Ѵ�.
	m_pGraphicDev->GetDepthStencilSurface(&m_pDepthStencilBuffer);

	if (bShadowMap)
	{
		m_bShadowMap = true;

		// ����Ÿ�ٰ� ������ ũ���� ���̹��۸� �����.
		if (FAILED(m_pGraphicDev->CreateDepthStencilSurface(iSizeX, iSizeY, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE,
			&m_pShadowDepthStencil, NULL)))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CTarget::SetUp_OnGraphicDev(const _uint & iIndex)
{
	if (nullptr == m_pGraphicDev)
		return E_FAIL;

	m_pGraphicDev->GetRenderTarget(iIndex, &m_pOldSurface);

	m_pGraphicDev->SetRenderTarget(iIndex, m_pRenderTarget);

	m_iIndex = iIndex;

	// ������ ���̸� �������� �������� ���̹��۸� ����ؾ��Ѵ�. �װ� �ƴϸ� �� ������ ���̹��۵��
	// ���⼭ �ø������� ���� �����ӿ� �׷ȴ� �׸��� ������ �����.
	if (m_bShadowMap)
	{
		m_pGraphicDev->SetDepthStencilSurface(m_pShadowDepthStencil);

		// �׷��� ���̹��ۿ� ���� ����ϱ����� ���� �����ӿ� �׷ȴ� ���� ������ �������Ѵ�.
		m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_ZBUFFER,
			0xFFFFFFFF, 1.f, 0);
	}
	else
		m_pGraphicDev->SetDepthStencilSurface(m_pDepthStencilBuffer);


	if (m_pBackBuff == m_pOldSurface)
		Safe_Release(m_pOldSurface);

	return NOERROR;
}

HRESULT CTarget::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName)
{
	pEffect->SetTexture(pConstantName, m_pTargetTexture);

	return NOERROR;
}

HRESULT CTarget::Release_OnGraphicDev(void)
{
	if (nullptr == m_pGraphicDev)
		return E_FAIL;

	m_pGraphicDev->SetRenderTarget(m_iIndex, m_pOldSurface); // ���� �÷����ִ� ����Ÿ������ �÷��д�.
	m_pGraphicDev->SetDepthStencilSurface(m_pDepthStencilBuffer);

	return NOERROR;
}

HRESULT CTarget::Clear_Target(void)
{
	if (nullptr == m_pGraphicDev)
		return E_FAIL;

	m_pGraphicDev->GetRenderTarget(0, &m_pOldSurface);

	m_pGraphicDev->SetRenderTarget(0, m_pRenderTarget);

	m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET, m_Color, 1.f, 0);

	m_pGraphicDev->SetRenderTarget(0, m_pOldSurface);

	m_pOldSurface->Release();

	return NOERROR;
}

HRESULT CTarget::Ready_DebugBuffer(_vec4 * pBufferInfo)
{
	m_pBufferCom = CView_Texture::Create(m_pGraphicDev, pBufferInfo);
	if (nullptr == m_pBufferCom)
		return E_FAIL;

	return NOERROR;
}

HRESULT CTarget::Render_DebugBuffer(void)
{
	m_pGraphicDev->SetTexture(0, m_pTargetTexture);

	m_pBufferCom->Render_Buffer();

	return NOERROR;
}

CTarget * CTarget::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color, bool bShadowMap)
{
	CTarget* pInstance = new CTarget(pGraphicDev);

	if (FAILED(pInstance->Ready_Target(iSizeX, iSizeY, Format, Color, bShadowMap)))
	{
		MSG_BOX(L"CTarget Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void CTarget::Free(void)
{
	DWORD dwRefCnt = 0;

	dwRefCnt = Engine::Safe_Release(m_pBufferCom);

	dwRefCnt = Engine::Safe_Release(m_pBackBuff);
	dwRefCnt = Engine::Safe_Release(m_pShadowDepthStencil);

	dwRefCnt = Engine::Safe_Release(m_pTargetTexture);
	dwRefCnt = Engine::Safe_Release(m_pRenderTarget);
	dwRefCnt = Engine::Safe_Release(m_pDepthStencilBuffer);

	dwRefCnt = Engine::Safe_Release(m_pGraphicDev);
}
