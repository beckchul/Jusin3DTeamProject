#include "Renderer.h"
#include "GameObject.h"
#include "Shader.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "InputDev.h"
#include "DataManager.h"
#include "TextureManager.h"

USING(Engine)

Engine::CRenderer::CRenderer(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pTargetMgr(CTarget_Manager::GetInstance())
	, m_bShowRenderTargets(FALSE)
	, m_vPixelOffSet(0.f, 0.f, 0.f, 0.f)
{
	m_pGraphicDev->AddRef();
	m_pTargetMgr->Add_Ref();
}

Engine::CRenderer::~CRenderer(void)
{

}

HRESULT Engine::CRenderer::Add_RenderList(RENDER eType, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_RenderList[eType].push_back(pGameObject);

	pGameObject->Add_Ref();

	return S_OK;
}

HRESULT Engine::CRenderer::Ready_Component(void)
{
	if (nullptr == m_pTargetMgr)
		return E_FAIL;

	D3DVIEWPORT9			ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);

	m_iSizeX = ViewPort.Width;
	m_iSizeY = ViewPort.Height;

	// �ȼ��� ũ��
	m_vPixelOffSet = _vec4(1 / (float)ViewPort.Width, 1 / (float)ViewPort.Height, 0, 0);

	int index = 0;

	// 16 ���ø��� ���� offset ��ġ.
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			m_vOffsets[index].x = (x - 1.5f) / m_iSizeX;
			m_vOffsets[index].y = (y - 1.5f) / m_iSizeY;
			index++;
		}
	}


	// ================ For. Create Shader ==================================
	if (FAILED(Create_Shader()))
	{
		MSG_BOX(L"Create_Shader Failed - CRenderer::Ready_Component");
		return E_FAIL;
	}

	// =========== For. Create RenderTarget =============================================
	if (FAILED(Create_RenderTarget(ViewPort)))
	{
		MSG_BOX(L"Create_RenderTarget Failed - CRenderer::Ready_Component");
		return E_FAIL;
	}

	// ===============For. Create Buff==============================================
	if (FAILED(Create_ViewTexBuffer(ViewPort)))
	{
		MSG_BOX(L"Create_ViewTexBuffer Failed - CRenderer::Ready_Component");
		return E_FAIL;
	}

	return S_OK;
}

void Engine::CRenderer::Render_GameObject(void)
{
	/*_vec3 campos = CDataManager::GetInstance()->Get_CamPoistion();
	printf("pos x : %f, pos y : %f, pos z : %f \n", campos.x, campos.y, campos.z);*/

	Render_Priority();

	/// ========= ���� ���̵� ===================================================

	// �������� ��ü���� ���۵�� �׸���.
	Render_Deferred();

#ifndef BadComputer

	// ��� �׷����� �ֵ��� ���� �׷�����.
	Render_Glow();

	// �׷��� Normal�� Depth�� �̿��� �ܰ����� �����Ѵ�.
	Render_Edge(L"Target_Edge_Normal", L"Target_Normal");
	Render_Edge(L"Target_Edge_Depth", L"Target_Depth");


#endif // !BadComputer

	Render_Light(); /* �������� ��ü���� Render_Deferred�� ���� ��� �׸��Ŀ� Render_Light�� ���� �� ������ ���� ���� �׷��� �Ѵ�.*/

	Render_Scene(); // Target_Color�� �׷����Ͱ� Target_Shade�� �׷��� ���� �ռ��Ͽ� ���� ����Ѵ�.

	/// =========================================================================

	Render_Default();  // Ÿ�ٿ� �ȱ׷����� ������ None_Alpha�� �ֵ� ����� ~~

	Render_Alpha();

	Render_Effect();

	Render_UI();



	Clear_RenderList();

	if (CInputDev::GetInstance()->OnceKeyDown(DIK_R))
		m_bShowRenderTargets = (m_bShowRenderTargets == TRUE) ? FALSE : TRUE;
	
	if (m_bShowRenderTargets)
	{
		m_pTargetMgr->Render_DebugBuffer(L"Target_Shadow");

		m_pTargetMgr->Render_DebugBuffer(L"Target_Color");
		m_pTargetMgr->Render_DebugBuffer(L"Target_Normal");
		m_pTargetMgr->Render_DebugBuffer(L"Target_Depth");
		m_pTargetMgr->Render_DebugBuffer(L"Target_SpecularIntensity");
		m_pTargetMgr->Render_DebugBuffer(L"Target_Depth2");

		m_pTargetMgr->Render_DebugBuffer(L"Target_Edge_Normal");
		m_pTargetMgr->Render_DebugBuffer(L"Target_Edge_Depth");

		m_pTargetMgr->Render_DebugBuffer(L"Target_GlowSources");
		m_pTargetMgr->Render_DebugBuffer(L"Target_DownFilter");
		m_pTargetMgr->Render_DebugBuffer(L"Target_Blur");
		m_pTargetMgr->Render_DebugBuffer(L"Target_Glow");

		m_pTargetMgr->Render_DebugBuffer(L"Target_Shade");
		m_pTargetMgr->Render_DebugBuffer(L"Target_Specular");

		m_pTargetMgr->Render_DebugBuffer(L"Target_Scene");
		m_pTargetMgr->Render_DebugBuffer(L"Target_DownScene");
		m_pTargetMgr->Render_DebugBuffer(L"Target_BlurScene");
		//m_pTargetMgr->Render_DebugBuffer(L"Target_Bloom");
	}

	
}

HRESULT CRenderer::GetGaussBlur5x5(DWORD dwD3DTexWidth, DWORD dwD3DTexHeight, D3DXVECTOR2 * avTexCoordOffset, D3DXVECTOR4 * avSampleWeight)
{
	float tu = 1.0f / (float)dwD3DTexWidth;
	float tv = 1.0f / (float)dwD3DTexHeight;

	float totalWeight = 0.0f;
	int index = 0;
	for (int x = -2; x <= 2; x++) {
		for (int y = -2; y <= 2; y++) {
			// ����� ���� �κ��� �Ұ�
			if (2 < abs(x) + abs(y)) continue;

			avTexCoordOffset[index] = D3DXVECTOR2(x * tu, y * tv);
			float fx = (FLOAT)x;
			float fy = (FLOAT)y;
			avSampleWeight[index].x = avSampleWeight[index].y =
				avSampleWeight[index].z = avSampleWeight[index].w
				= expf(-(fx*fx + fy*fy) / (2 * 1.0f));
			totalWeight += avSampleWeight[index].x;

			index++;
		}
	}

	// ����ġ �հ�� 1.0
	for (int i = 0; i < index; i++) avSampleWeight[i] *= 1.0f / totalWeight;

	return S_OK;
}

HRESULT CRenderer::Create_RenderTarget(D3DVIEWPORT9 ViewPort)
{
	// ȭ����� ����
	float fWidth = (ViewPort.Width / 8.f) * 1.12f;
	float fHeight = (ViewPort.Height / 6.f) * 1.12f;

	////////////////////////////////////////////////////////////////////////////////////
	// For.Shadow Map
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Shadow"
		, 4096, 4096, D3DFMT_R32F, D3DXCOLOR(1.f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Shadow", &_vec4(0.f, 0.f, fWidth, fHeight))))
		return E_FAIL;
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// For.Common (�Ϲ������� �׷����� �͵�)
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Color"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Color", &_vec4(fWidth * 1.f, fHeight * 0, fWidth, fHeight))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Normal"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Normal", &_vec4(fWidth * 1.f, fHeight * 1.f, fWidth, fHeight))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Depth"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Depth", &_vec4(fWidth * 1.f, fHeight * 2.f, fWidth, fHeight))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_SpecularIntensity"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A8B8G8R8, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_SpecularIntensity", &_vec4(fWidth * 1.f, fHeight * 3.f, fWidth, fHeight))))
		return E_FAIL;


	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Depth2"
		, ViewPort.Width, ViewPort.Height, D3DFMT_R32F, D3DXCOLOR(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Depth2", &_vec4(fWidth * 1.f, fHeight * 4.f, fWidth, fHeight))))
		return E_FAIL;
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// For.Edge(�ܰ��� ����Ÿ��)
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Edge_Normal"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Edge_Normal", &_vec4(fWidth * 2.f, fHeight * 0, fWidth, fHeight))))
		return E_FAIL;

	// For.Edge Depth
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Edge_Depth"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Edge_Depth", &_vec4(fWidth * 2.f, fHeight * 1.f, fWidth, fHeight))))
		return E_FAIL;
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	// For.GlowSources
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_GlowSources"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_GlowSources", &_vec4(fWidth * 3.f, fHeight * 0.f, fWidth, fHeight))))
		return E_FAIL;

	// For. DownFilter
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_DownFilter"
		, ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_DownFilter", &_vec4(fWidth * 3.f, fHeight * 1.f, fWidth, fHeight))))
		return E_FAIL;

	// For. Blur
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Blur"
		, ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Blur", &_vec4(fWidth * 3.f, fHeight * 2.f, fWidth, fHeight))))
		return E_FAIL;

	// For.Glow
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Glow"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Glow", &_vec4(fWidth * 3.f, fHeight * 3.f, fWidth, fHeight))))
		return E_FAIL;

	////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////
	// For.Shade(���ݻ籤)
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Shade"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Shade", &_vec4(fWidth * 4.f, fHeight * 0.f, fWidth, fHeight))))
		return E_FAIL;

	// For.Specular(���ݻ籤)
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Specular"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Specular", &_vec4(fWidth * 4.f, fHeight * 1.f, fWidth, fHeight))))
		return E_FAIL;
	////////////////////////////////////////////////////////////////////////////////////
	

	////////////////////////////////////////////////////////////////////////////////////
	// For. Scene (����� �׷������� Ÿ�ٿ� ���� �׸���)
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Scene"
		, ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Scene", &_vec4(fWidth * 5.f, fHeight * 0.f, fWidth, fHeight))))
		return E_FAIL;

	// For. DownScene
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_DownScene"
		, ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_DownScene", &_vec4(fWidth * 5.f, fHeight * 1.f, fWidth, fHeight))))
		return E_FAIL;

	// For. Target_BlurScene
	if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_BlurScene"
		, ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_BlurScene", &_vec4(fWidth * 5.f, fHeight * 2.f, fWidth, fHeight))))
		return E_FAIL;

	//// For. Bloom
	//if (FAILED(m_pTargetMgr->Add_RenderTarget(m_pGraphicDev, L"Target_Bloom"
	//	, ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;
	//if (FAILED(m_pTargetMgr->Ready_DebugBuffer(L"Target_Bloom", &_vec4(fWidth * 5.f, fHeight * 3.f, fWidth, fHeight))))
	//	return E_FAIL;
	//////////////////////////////////////////////////////////////////////////////////
	

	
	// ================MRT_CNDS===========================================
	if (FAILED(m_pTargetMgr->Add_MRT(L"MRT_CNDS", L"Target_Color")))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Add_MRT(L"MRT_CNDS", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Add_MRT(L"MRT_CNDS", L"Target_Depth")))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Add_MRT(L"MRT_CNDS", L"Target_SpecularIntensity")))
		return E_FAIL;


	// ================MRT_Light===========================================
	if (FAILED(m_pTargetMgr->Add_MRT(L"MRT_Light", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTargetMgr->Add_MRT(L"MRT_Light", L"Target_Specular")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Create_ViewTexBuffer(D3DVIEWPORT9 ViewPort)
{
	m_pTempBuff = CView_Texture::Create(m_pGraphicDev, &_vec4(0.f, 0.f, (float)ViewPort.Width, (float)ViewPort.Height));
	if (nullptr == m_pTempBuff)
		return E_FAIL;

	m_pDownSizeBuff = CView_Texture::Create(m_pGraphicDev, &_vec4(0.f, 0.f, (float)ViewPort.Width / 4.f, (float)ViewPort.Height / 4.f));
	if (nullptr == m_pDownSizeBuff)
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Create_Shader(void)
{
	CShader* pShader = nullptr;

	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_ShadowMap.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_ShadowMap", pShader));

	// ======= For. RENDER_STATICMESH_NONEALPHA ================================================
	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_StaticMesh.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_StaticMesh", pShader));
	// =====================================================================================

	// ======= For. RENDER_DYNAMICMESH_NONEALPHA ================================================
	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_DynamicMesh.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_DynamicMesh", pShader));
	// =====================================================================================


	// ======= For. Render_Deferred ================================================
	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_Light.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_Light", pShader));


	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_Blend.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_Blend", pShader));


	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_GaussianFilter.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_GaussianFilter", pShader));


	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_EdgeDetection.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_EdgeDetection", pShader));

	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_FinalRendering.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_FinalRendering", pShader));

	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_BrightFilter.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_BrightFilter", pShader));


	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_DownFilter.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_DownFilter", pShader));

	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_Bloom.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_Bloom", pShader));


	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_CreateGlowSource.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_CreateGlowSource", pShader));

	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_Blur.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_Blur", pShader));
	// =====================================================================================


	// ======= For. Render_UI ================================================

	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_UI.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_UI", pShader));

	// =====================================================================================


	// ======= For. Render_Alpha(Effect) ================================================

	pShader = CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_Effect.hpp");
	if (nullptr == pShader)
		return E_FAIL;
	m_mapShader.insert(MAPSHADER::value_type(L"Shader_Effect", pShader));

	// =====================================================================================

	return NOERROR;
}

CShader * CRenderer::Find_Shader(const _tchar* pShaderTag)
{
	MAPSHADER::iterator iter = find_if(m_mapShader.begin(), m_mapShader.end(), CTag_Finder(pShaderTag));

	if (iter == m_mapShader.end())
		return nullptr;

	return iter->second;
}

void Engine::CRenderer::Render_Priority(void)
{
	RENDERLIST::iterator	iter = m_RenderList[RENDER_PRIORITY].begin();
	RENDERLIST::iterator	iter_end = m_RenderList[RENDER_PRIORITY].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render_GameObject();
	}
}

void CRenderer::Render_StaticMesh_NoneAlpha(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	RENDERLIST::iterator	iter = m_RenderList[RENDER_STATICMESH_NONEALPHA].begin();
	RENDERLIST::iterator	iter_end = m_RenderList[RENDER_STATICMESH_NONEALPHA].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render_GameObject(pArgEffect, uPassIdx); // staticMesh �븻�������� �׷����� ���̴� �����ϱ�~
	}
}

void CRenderer::Render_DynamicMesh_NoneAlpha(LPD3DXEFFECT pArgEffect, _bool bFirstRender, _uint uPassIdx)
{
	RENDERLIST::iterator	iter = m_RenderList[RENDER_DYNAMICMESH_NONEALPHA].begin();
	RENDERLIST::iterator	iter_end = m_RenderList[RENDER_DYNAMICMESH_NONEALPHA].end();

	if (bFirstRender)
	{
		for (; iter != iter_end; ++iter)
		{
			(*iter)->FirstRender_GameObject();					// �߰�: FirstRender : �ִϸ��̼� �ι� ���°��� ������
			(*iter)->Render_GameObject(pArgEffect, uPassIdx); // dynamicMesh �븻�������� �׷����� ���̴� �����ϱ�~
		}
	}
	else
	{
		for (; iter != iter_end; ++iter)
		{
			(*iter)->FirstRender_GameObject(true);
			(*iter)->Render_GameObject(pArgEffect, uPassIdx); // dynamicMesh �븻�������� �׷����� ���̴� �����ϱ�~
		}
	}
}

void CRenderer::Render_Default(void)
{
	RENDERLIST::iterator	iter = m_RenderList[RENDER_DEFAULT].begin();
	RENDERLIST::iterator	iter_end = m_RenderList[RENDER_DEFAULT].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render_GameObject();
	}
}

_bool Compare(CGameObject* pSrc, CGameObject* pDest)
{
	return pSrc->Get_ViewZ() > pDest->Get_ViewZ();
}

void Engine::CRenderer::Render_Alpha(void)
{
	m_RenderList[RENDER_ALPHA].sort(Compare);

	RENDERLIST::iterator	iter = m_RenderList[RENDER_ALPHA].begin();
	RENDERLIST::iterator	iter_end = m_RenderList[RENDER_ALPHA].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render_GameObject();
	}
}

void Engine::CRenderer::Render_Effect(void)
{
	m_RenderList[RENDER_EFFECT].sort(Compare);

	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	pShader = Find_Shader(L"Shader_Effect");
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();

	pEffect->Begin(nullptr, 0);

	RENDERLIST::iterator	iter = m_RenderList[RENDER_EFFECT].begin();
	RENDERLIST::iterator	iter_end = m_RenderList[RENDER_EFFECT].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render_GameObject(pEffect);
	}

	pEffect->End();
	pEffect->Release();
}

void Engine::CRenderer::Render_UI(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

	D3DXMATRIX		matOldView, matOldProj, matCurProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);

	D3DXMatrixOrthoLH(&matCurProj, (float)m_iSizeX, (float)m_iSizeY, 0.f, 1.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matCurProj);

	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	pShader = Find_Shader(L"Shader_UI");
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();

	pEffect->Begin(nullptr, 0);
	
	RENDERLIST::iterator	iter = m_RenderList[RENDER_UI].begin();
	RENDERLIST::iterator	iter_end = m_RenderList[RENDER_UI].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render_GameObject(pEffect);
	}

	pEffect->End();
	pEffect->Release();
	
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void CRenderer::Render_SkyBox(void)
{
}

void Engine::CRenderer::Clear_RenderList(void)
{
	for (_ulong i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderList[i].begin(), m_RenderList[i].end(), CRelease_Single());
		m_RenderList[i].clear();
	}
}

void CRenderer::Render_Deferred(void)
{
	// ��ġ�� ����Ǿ��ִ� ����Ÿ���� ��ü�Ѵ�. (����۸� ����, 0 : Į��, 1: ��� 2: ����, 3: ����ŧ������ ���Ѵ�.)
	// ���⼭ Begin �������ְ� ���� Ŭ���� �ȿ��� ������̺� �� �ְ� commitChange�ؼ� ����

	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	_int& iSceneID = CDataManager::GetInstance()->Get_SceneID();

#ifndef BadComputer


	if (iSceneID == 2 || iSceneID == 3)
	{
		// �� ���� �׸��� ���� ���� �׸�.(1. �׸��ڸ� �����)
		m_pTargetMgr->Begin_SingleTarget(L"Target_Shadow");

		pShader = m_mapShader[L"Shader_ShadowMap"];
		if (nullptr == pShader)
			return;

		pEffect = pShader->Get_EffectHandle();

		pEffect->SetMatrix("g_matLightView", &CDataManager::GetInstance()->Get_LightViewMatrix());
		pEffect->SetMatrix("g_matLightProj", &CDataManager::GetInstance()->Get_LightProjMatrix());

		pEffect->Begin(nullptr, 0);
		Render_DynamicMesh_NoneAlpha(pEffect, false, 100); // ���̳��� �޽���.
		Render_StaticMesh_NoneAlpha(pEffect, 100);		   // ���⸸ ���� �� �׷�����;
		pEffect->End();


		m_pTargetMgr->End_SingleTarget(L"Target_Shadow");
	}

#endif // !BadComputer

	m_pTargetMgr->Begin_MRT(L"MRT_CNDS"); // Color, Normal, Depth, SpecIntensity �ѹ��� 4�� ����Ÿ�� �ö�

	// For. DynamicMesh Render
	pShader = m_mapShader[L"Shader_DynamicMesh"];
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();

	iSceneID = CDataManager::GetInstance()->Get_SceneID();

	if (iSceneID == 4)
		pEffect->SetBool("bShadowAdd", false);
	else
		pEffect->SetBool("bShadowAdd", true);

	// pass ID�� ���ο��� ���Ѵ�.
	pEffect->Begin(nullptr, 0);
#ifdef BadComputer
	Render_DynamicMesh_NoneAlpha(pEffect, true); // �븻����, �׸��� ȿ�� ����.
#else
	pEffect->SetMatrix("g_matLightView", &CDataManager::GetInstance()->Get_LightViewMatrix());
	pEffect->SetMatrix("g_matLightProj", &CDataManager::GetInstance()->Get_LightProjMatrix());
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Shadow", "g_ShadowTexture");
	Render_DynamicMesh_NoneAlpha(pEffect, true); // 2. �׸��� �帮���.
#endif
	pEffect->End();

	// For. StaticMesh Render
	pShader = m_mapShader[L"Shader_StaticMesh"];
	if (nullptr == pShader)
		return;

	/// ������Ʈ ���� camPos�ʿ�.
	/*_vec4 camPos = _vec4(CDataManager::GetInstance()->Get_CamPoistion(), 1.f);

	pEffect->SetVector("g_CamPosition", &camPos);*/
	
	pEffect = pShader->Get_EffectHandle();

	pEffect->Begin(nullptr, 0);
#ifdef BadComputer
	Render_StaticMesh_NoneAlpha(pEffect); // �븻����, �׸��� ȿ�� ����.
#else
	pEffect->SetMatrix("g_matLightView", &CDataManager::GetInstance()->Get_LightViewMatrix());
	pEffect->SetMatrix("g_matLightProj", &CDataManager::GetInstance()->Get_LightProjMatrix());
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Shadow", "g_ShadowTexture");
	Render_StaticMesh_NoneAlpha(pEffect); // 2. �׸��� �帮���.
#endif
	pEffect->End();

	m_pTargetMgr->End_MRT(L"MRT_CNDS");


	m_pTargetMgr->Begin_SingleTarget(L"Target_Depth2");

	pShader = m_mapShader[L"Shader_StaticMesh"];
	if (nullptr == pShader)
		return;
	pEffect = pShader->Get_EffectHandle();

	if (iSceneID == 4)
		pEffect->SetBool("bShadowAdd", false);
	else
		pEffect->SetBool("bShadowAdd", true);

	pEffect->Begin(nullptr, 0);
	Render_StaticMesh_NoneAlpha(pEffect, 200);
	pEffect->End();

	m_pTargetMgr->End_SingleTarget(L"Target_Depth2");
}

void CRenderer::Render_Glow(void)
{
	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	// 1. rgb�� a�� ������ Glow Sources�� �����.
	m_pTargetMgr->Begin_SingleTarget(L"Target_GlowSources");

	pShader = m_mapShader[L"Shader_CreateGlowSource"];
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();

	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Color", "g_SampleTexture");

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);
	m_pTempBuff->Render_Buffer();
	pEffect->EndPass();
	pEffect->End();

	m_pTargetMgr->End_SingleTarget(L"Target_GlowSources");

	// 2. GlowSources�� �۰� ����ؼ� �׸���.
	Render_DownSampling(L"Target_DownFilter", L"Target_GlowSources");	

	// 3. ����Ѱ��� ������.
	Render_Blur(L"Target_Blur", L"Target_DownFilter", 1, 4); // 1/4�� ���.

	// 4. ���� Color�� rgb�� Target_GlowSources�� �׷��� ���� ���ؼ� ���� �׸���.
	m_pTargetMgr->Begin_SingleTarget(L"Target_Glow");

	pShader = m_mapShader[L"Shader_CreateGlowSource"];
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();

	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Color", "g_SampleTexture");
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Blur", "g_SampleTexture2");
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture");

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(1);
	m_pTempBuff->Render_Buffer();
	pEffect->EndPass();
	pEffect->End();

	m_pTargetMgr->End_SingleTarget(L"Target_Glow");
}

void CRenderer::Render_Edge(const _tchar* pTargetTag, const _tchar* pTextureTag)
{
	m_pTargetMgr->Begin_SingleTarget(pTargetTag);

	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	pShader = m_mapShader[L"Shader_EdgeDetection"];
	if (nullptr == pShader)
		return;
	
	pEffect = pShader->Get_EffectHandle();
	
	pEffect->AddRef();
	
	m_pTargetMgr->SetUp_OnShader(pEffect, pTextureTag, "g_SrcTexture");
	pEffect->SetVector("gPixelOffSet", &m_vPixelOffSet);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pTempBuff->Render_Buffer();

	pEffect->EndPass();

	pEffect->End();

	pEffect->Release();

	m_pTargetMgr->End_SingleTarget(pTargetTag);
}

void CRenderer::Render_Filter(const _tchar * pTargetTag, const _tchar * pTextureTag)
{
	m_pTargetMgr->Begin_SingleTarget(pTargetTag);

	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	pShader = m_mapShader[L"Shader_GaussianFilter"];
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();

	m_pTargetMgr->SetUp_OnShader(pEffect, pTextureTag, "g_SampleTexture");
	pEffect->SetVector("texScaler", &m_vPixelOffSet);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0); // X������ ��

	m_pTempBuff->Render_Buffer();

	pEffect->EndPass();

	// X������ ���� �� Ÿ���� DiffuseSampler�� ����.
	m_pTargetMgr->SetUp_OnShader(pEffect, pTargetTag, "g_SampleTexture");
	pEffect->CommitChanges();

	pEffect->BeginPass(1); // Y������ ��

	m_pTempBuff->Render_Buffer();

	pEffect->EndPass();

	pEffect->End();

	m_pTargetMgr->End_SingleTarget(pTargetTag);
}

void CRenderer::Render_Blur(const _tchar* pTargetTag, const _tchar* pTextureTag, int iBlurCount, int iDownSampleValue /*= 1*/)
{
	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	pShader = m_mapShader[L"Shader_Blur"];
	if (nullptr == pShader)
		return;


	m_pTargetMgr->Begin_SingleTarget(pTargetTag);

	pEffect = pShader->Get_EffectHandle();

	for (int i = 0; i < iBlurCount; ++i)
	{
		if (0 == i) // ù��° �������� Texture�� Source�� ��.
		{
			m_pTargetMgr->SetUp_OnShader(pEffect, pTextureTag, "m_TxDif");
		}
		else if(i >= 1)// �ι�° �̻���ʹ� BlurŸ����ü�� �� ���Ѵ�.
		{
			m_pTargetMgr->SetUp_OnShader(pEffect, pTargetTag, "m_TxDif");
		}

		pEffect->SetFloat("m_TexW", float(m_iSizeX) / float(iDownSampleValue));
		pEffect->SetFloat("m_TexH", float(m_iSizeY) / float(iDownSampleValue));

		pEffect->Begin(nullptr, 0);

		pEffect->BeginPass(0); // X

		if (1 == iDownSampleValue)
			m_pTempBuff->Render_Buffer();
		else
			m_pDownSizeBuff->Render_Buffer(); // Down ���ø��������.

		pEffect->EndPass();

		m_pTargetMgr->SetUp_OnShader(pEffect, pTargetTag, "m_TxDif");
		pEffect->SetFloat("m_TexW", float(m_iSizeX) / float(iDownSampleValue));
		pEffect->SetFloat("m_TexH", float(m_iSizeY) / float(iDownSampleValue));
		pEffect->CommitChanges();

		float ff = float(m_iSizeX) / float(iDownSampleValue);

		pEffect->BeginPass(1); // Y������ ��

		if (1 == iDownSampleValue)
			m_pTempBuff->Render_Buffer();
		else
			m_pDownSizeBuff->Render_Buffer(); // Down ���ø��������.

		pEffect->EndPass();

		pEffect->End();
	}

	m_pTargetMgr->End_SingleTarget(pTargetTag);
}

void CRenderer::Render_DownSampling(const _tchar* pDownTargetTag, const _tchar* pTextureTag)
{
	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	m_pTargetMgr->Begin_SingleTarget(pDownTargetTag);

	pShader = m_mapShader[L"Shader_DownFilter"];
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();

	m_pTargetMgr->SetUp_OnShader(pEffect, pTextureTag, "g_SrcTexture");
	pEffect->SetValue("g_avSampleOffsets", m_vOffsets, sizeof(m_vOffsets));

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);
	m_pDownSizeBuff->Render_Buffer();
	pEffect->EndPass();
	pEffect->End();

	m_pTargetMgr->End_SingleTarget(pDownTargetTag);
}

void CRenderer::Render_Bloom(const _tchar * pBloomTarget, const _tchar * pTextureTag)
{
	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	m_pTargetMgr->Begin_SingleTarget(pTextureTag);

	pShader = m_mapShader[L"Shader_Bloom"];
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();

	m_pTargetMgr->SetUp_OnShader(pEffect, pBloomTarget, "g_SrcTexture");
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture");

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);
	m_pDownSizeBuff->Render_Buffer();
	pEffect->EndPass();
	pEffect->End();

	m_pTargetMgr->End_SingleTarget(pTextureTag);
}
 
void CRenderer::Render_Light(void)
{
	m_pTargetMgr->Begin_MRT(L"MRT_Light");

	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	pShader = m_mapShader[L"Shader_Light"];
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();

	pEffect->AddRef();

	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Normal", "g_NormalTexture");
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture");
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_SpecularIntensity", "g_SpecularIntensityTexture");

	pEffect->Begin(nullptr, 0); // Shader�� Begin �Լ��� �ּ������� ȣ������

	pEffect->SetFloat("g_fFar", PLANE_FAR);
	CLight_Manager::GetInstance()->Render_Light(pEffect);// ������ �׷���.

	pEffect->End();

	pEffect->Release();

	m_pTargetMgr->End_MRT(L"MRT_Light");
}

void CRenderer::Render_Scene(void)
{
	// Target_Scene�� �÷��ΰ� ���⿡ �ѹ��׸��� ����ۿ��� �ѹ� �׸���.

	m_pTargetMgr->Begin_SingleTarget(L"Target_Scene");

	CShader* pShader = nullptr;
	LPD3DXEFFECT pEffect = nullptr;

	pShader = m_mapShader[L"Shader_Blend"];
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();

	pEffect->AddRef();

	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Color", "g_ColorTexture"); // albedo
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Shade", "g_ShadeTexture"); // diffuse
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Specular", "g_SpecularTexture"); // spec
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Glow", "g_GlowSourcesTexture"); // glow
	
	pEffect->Begin(nullptr, 0);

#ifdef BadComputer // ����Ʈ ���μ��� ó�� X
	pEffect->BeginPass(1);
#else // ����Ʈ ���μ��� ó�� O
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Edge_Normal", "g_EdgeNormalTexture"); // post effect
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Edge_Depth", "g_EdgeDepthTexture");
	pEffect->BeginPass(0);
#endif

	m_pTempBuff->Render_Buffer();

	pEffect->EndPass();

	pEffect->End();

	pEffect->Release();

	m_pTargetMgr->End_SingleTarget(L"Target_Scene");


#ifndef BadComputer
	// 1. �ϴ� Scene�� DownSize�� �����.
	Render_DownSampling(L"Target_DownScene", L"Target_Scene");


	//// 1-1 DownSize�� Target_DownScene�� ���ó��.
	//Render_Bloom(L"Target_DownScene", L"Target_Bloom");


	// 2. DownSize�� �� Target_DownScene�� ��ó���Ѵ�.
	Render_Blur(L"Target_BlurScene", L"Target_DownScene", 1, 4);
#endif
	
	// ����ۿ� ����� �������Ѵ�.

	pShader = nullptr;
	pEffect = nullptr;

	pShader = m_mapShader[L"Shader_FinalRendering"];
	if (nullptr == pShader)
		return;

	pEffect = pShader->Get_EffectHandle();

	pEffect->AddRef();

	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_BlurScene", "g_BlurTexture");
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Scene", "g_SceneTexture");
	m_pTargetMgr->SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture");

	_vec4 BackColor = CDataManager::GetInstance()->Get_BackColor();
	pEffect->SetVector("g_BackColor", &BackColor);

	_vec4 FadeColor = CDataManager::GetInstance()->Get_FadeColor();
	pEffect->SetVector("g_FadeColor", &FadeColor);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pTempBuff->Render_Buffer();

	pEffect->EndPass();

	pEffect->End();

	pEffect->Release();
}

CComponent* Engine::CRenderer::Clone(void)
{
	Add_Ref();

	return this;
}

CRenderer* Engine::CRenderer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRenderer* pInstance = new CRenderer(pGraphicDev);

	if (FAILED(pInstance->Ready_Component()))
	{
		MSG_BOX(L"CRenderer Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::CRenderer::Free(void)
{
	DWORD dwRefCnt = 0;
	for (_ulong i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderList[i].begin(), m_RenderList[i].end(), CRelease_Single());
		m_RenderList[i].clear();
	}

	for_each(m_mapShader.begin(), m_mapShader.end(), CRelease_Pair());
	m_mapShader.clear();

	dwRefCnt = Engine::Safe_Release(m_pTempBuff);
	dwRefCnt = Engine::Safe_Release(m_pDownSizeBuff);

	dwRefCnt = Engine::Safe_Release(m_pTargetMgr);
	dwRefCnt = Engine::Safe_Release(m_pGraphicDev);
}

