#include "GraphicDev.h"

USING(Engine)

IMPLEMENT_SINGLETON(Engine::CGraphicDev)

Engine::CGraphicDev::CGraphicDev(void)
: m_pSDK(nullptr)
, m_pGraphicDev(nullptr)
{

}

Engine::CGraphicDev::~CGraphicDev(void)
{
	
}

HRESULT Engine::CGraphicDev::Ready_GraphicDev(WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY)
{
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9		devicecaps;
	ZeroMemory(&devicecaps, sizeof(D3DCAPS9));

	if(FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &devicecaps)))
	{
		MSG_BOX(L"GetDeviceCaps Failed");
		return E_FAIL;
	}

	DWORD		vp;

	if(devicecaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	vp |= D3DCREATE_MULTITHREADED;

	D3DPRESENT_PARAMETERS	d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	SetParameters(d3dpp, Mode, hWnd, wSizeX, wSizeY);

	if(FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL
		, hWnd, vp, &d3dpp, &m_pGraphicDev)))
	{
		MSG_BOX(L"CreateDevice Failed");
		return E_FAIL;
	}

	return S_OK;
}

void Engine::CGraphicDev::SetParameters(D3DPRESENT_PARAMETERS& d3dpp, WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY)
{
	d3dpp.BackBufferWidth = wSizeX;
	d3dpp.BackBufferHeight = wSizeY;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;

	d3dpp.MultiSampleQuality = 0;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.EnableAutoDepthStencil = TRUE;

	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = Mode;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

void Engine::CGraphicDev::Free(void)
{
	DWORD			dwRefCnt = 0;
#ifndef Demonstration
	TCHAR			szTemp[128];
#endif
	if (dwRefCnt = Engine::Safe_Release(m_pGraphicDev))
	{
#ifndef Demonstration
		wsprintf(szTemp, L"그래픽 디바이스 : %d개!!!", dwRefCnt);
		//MSG_BOX(szTemp);
#endif
	for (_uint i = 0; i < dwRefCnt; ++i)
		Engine::Safe_Release(m_pGraphicDev);
	}

	if (dwRefCnt = Engine::Safe_Release(m_pSDK))
	{
#ifndef Demonstration
		wsprintf(szTemp, L"m_pSDK  : %d개 ㅠㅠ", dwRefCnt);
		//MSG_BOX(szTemp);
#endif
	for (_uint i = 0; i < dwRefCnt; ++i)
		Engine::Safe_Release(m_pSDK);
	}
}
