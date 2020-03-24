#ifndef HierarchyLoader_h__
#define HierarchyLoader_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CHierarchyLoader : public ID3DXAllocateHierarchy
{
private:
	explicit CHierarchyLoader(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath);
	virtual ~CHierarchyLoader(void);
public:
	_ulong AddRef(void) {
		return ++m_dwRefCnt;}
public:
	HRESULT Ready_HierarchyLoader(void);
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer);
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree); 
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree);
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	const _tchar*		m_pFilePath;
	_ulong				m_dwRefCnt;
private:
	void Allocate_Name(char** ppSrcName, const char* pDestName); // 새로만든 뼈에 이름을 채워주기위한 함수.
public: 
	static CHierarchyLoader* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath);
public:
	_ulong Release(void);
	

};

END


#endif // HierarchyLoader_h__
