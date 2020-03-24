#ifndef Renderer_h__
#define Renderer_h__

#include "Component.h"

BEGIN(Engine)


class CView_Texture;
class CTarget_Manager;
class CGameObject;
class CShader;
class ENGINE_DLL CRenderer : public CComponent
{
public:
	enum RENDER {RENDER_PRIORITY, RENDER_DYNAMICMESH_NONEALPHA, RENDER_STATICMESH_NONEALPHA, RENDER_STATICMESH_CARTOON
				, RENDER_DEFAULT, RENDER_ALPHA, RENDER_EFFECT, RENDER_UI, RENDER_SKYBOXMESH, RENDER_END};
private:
	enum { 
			NUM_STAR_TEXTURES = 8, // 광선 텍스처 개수
			MAX_SAMPLES = 16, // 샘플러 최대 개수
		};
private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRenderer(void);
public:
	HRESULT Add_RenderList(RENDER eType, CGameObject* pGameObject);
public:
	virtual HRESULT Ready_Component(void);
	void Render_GameObject(void);
private:
	LPDIRECT3DDEVICE9				m_pGraphicDev = nullptr;
private:
	CTarget_Manager*				m_pTargetMgr = nullptr;

	// Shader 파일을 renderer에서 통합적으로 관리하기 위해서 map을 사용한다.
	typedef unordered_map<const _tchar*, CShader*> MAPSHADER;
	MAPSHADER						m_mapShader;

private:
	_vec2 m_vOffsets[MAX_SAMPLES];
	//_matrix m_matLightView;
	//_matrix m_matLightProj;

private:
	_uint							m_iSizeX = 0; // 직교 투영때 필요한 WINCX, WINCY
	_uint							m_iSizeY = 0;
	_vec4							m_vPixelOffSet;

private:
	list<CGameObject*>				m_RenderList[RENDER_END];
	typedef list<CGameObject*>		RENDERLIST;	
private:
	CView_Texture*				m_pTempBuff = nullptr; // 임의의 버퍼
	CView_Texture*				m_pDownSizeBuff = nullptr; // 1/4 축소 버퍼

private:
	bool						m_bShowRenderTargets = true;
private:
	HRESULT GetGaussBlur5x5(DWORD dwD3DTexWidth, DWORD dwD3DTexHeight,
		D3DXVECTOR2* avTexCoordOffset, D3DXVECTOR4* avSampleWeight);
private:
	HRESULT Create_RenderTarget(D3DVIEWPORT9 ViewPort);
	HRESULT Create_ViewTexBuffer(D3DVIEWPORT9 ViewPort);
	HRESULT Create_Shader(void);
	CShader* Find_Shader(const _tchar* pShaderTag);
private:
	void Render_Priority(void);
	void Render_StaticMesh_NoneAlpha(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	void Render_DynamicMesh_NoneAlpha(LPD3DXEFFECT pArgEffect = nullptr, _bool bFirstRender = false, _uint uPassIdx = 0);
	void Render_Default(void);
	void Render_Alpha(void);
	void Render_Effect(void);
	void Render_UI(void);
	void Render_SkyBox(void);
	void Clear_RenderList(void);
private:
	void Render_Deferred(void);
	void Render_Glow(void); // 스스로 빛나는 애들은 따로 여기서 그려짐.
	void Render_Edge(const _tchar* pTargetTag, const _tchar* pTextureTag); // 외각선을 그린다.
	void Render_Filter(const _tchar* pTargetTag, const _tchar* pTextureTag);
	void Render_Blur(const _tchar* pTargetTag, const _tchar* pTextureTag, int iBlurCount, int iDownSampleValue = 1);
	void Render_DownSampling(const _tchar* pDownTargetTag, const _tchar* pTextureTag); // down Target에 그려진다.
	void Render_Bloom(const _tchar* pBloomTarget, const _tchar* pTextureTag); // down Target에 그려진다.
	void Render_Light(void);
	void Render_Scene(void);
public:
	virtual CComponent* Clone(void);
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

END

#endif // Renderer_h__
