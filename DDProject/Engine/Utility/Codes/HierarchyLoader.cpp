#include "HierarchyLoader.h"

USING(Engine)

Engine::CHierarchyLoader::CHierarchyLoader(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath)
: m_pGraphicDev(pGraphicDev)
, m_pFilePath(pFilePath)
, m_dwRefCnt(0)
{
	m_pGraphicDev->AddRef();
}

Engine::CHierarchyLoader::~CHierarchyLoader(void)
{

}

HRESULT Engine::CHierarchyLoader::Ready_HierarchyLoader(void)
{
	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::CreateFrame(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
	D3DXFRAME_DERIVED*			pNewFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pNewFrame, sizeof(D3DXFRAME_DERIVED));

	if(nullptr != Name)
		Allocate_Name(&pNewFrame->Name, Name);

	pNewFrame->CombinedTransformationMatrix = *D3DXMatrixIdentity(&pNewFrame->TransformationMatrix);

	*ppNewFrame = pNewFrame;

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::CreateMeshContainer(THIS_ LPCSTR Name
														   , CONST D3DXMESHDATA *pMeshData
														   , CONST D3DXMATERIAL *pMaterials
														   , CONST D3DXEFFECTINSTANCE *pEffectInstances
														   , DWORD NumMaterials
														   , CONST DWORD *pAdjacency
														   , LPD3DXSKININFO pSkinInfo
														   , LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED*			pNewMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pNewMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	if(nullptr != Name)
		Allocate_Name(&pNewMeshContainer->Name, Name);

	pNewMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	LPD3DXMESH			pMesh = pMeshData->pMesh;
	pMesh->AddRef();

	_ulong dwMeshFVF = pMesh->GetFVF();

	// 폴리곤 갯수를 얻어온다.
	_ulong dwNumFaces = pMesh->GetNumFaces();
		
	pNewMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3];

	memcpy(pNewMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);

	if(!(dwMeshFVF & D3DFVF_NORMAL))
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwMeshFVF | D3DFVF_NORMAL, m_pGraphicDev, &pNewMeshContainer->MeshData.pMesh);

		D3DXComputeNormals(pNewMeshContainer->MeshData.pMesh, pNewMeshContainer->pAdjacency);

		Engine::Safe_Release(pMesh);
	}
	else
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwMeshFVF, m_pGraphicDev, &pNewMeshContainer->MeshData.pMesh);

		Engine::Safe_Release(pMesh);
	}

	pNewMeshContainer->NumMaterials = NumMaterials == 0 ? 1 : NumMaterials;

	pNewMeshContainer->pMaterials = new D3DXMATERIAL[pNewMeshContainer->NumMaterials];
	pNewMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pNewMeshContainer->NumMaterials]();
	pNewMeshContainer->ppNormalTextures = new LPDIRECT3DTEXTURE9[pNewMeshContainer->NumMaterials]();
	pNewMeshContainer->ppSpecTextures = new LPDIRECT3DTEXTURE9[pNewMeshContainer->NumMaterials]();
	pNewMeshContainer->ppEmissiveTextures = new LPDIRECT3DTEXTURE9[pNewMeshContainer->NumMaterials]();

	if(0 != NumMaterials)
	{
		memcpy(pNewMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * pNewMeshContainer->NumMaterials);

		_tchar szNormalTex[128] = L"";
		_tchar szSpecTex[128] = L"";

		for (_ulong i = 0; i < pNewMeshContainer->NumMaterials; ++i)
		{
			_tchar			szFileName[128] = L"";
			_tchar			szFullPath[128] = L"";

			MultiByteToWideChar(CP_ACP, 0, pNewMeshContainer->pMaterials[i].pTextureFilename, strlen(pNewMeshContainer->pMaterials[i].pTextureFilename)
				, szFileName, 128);

			lstrcpy(szFullPath, m_pFilePath);
			lstrcat(szFullPath, szFileName);

			lstrcpy(pNewMeshContainer->szTextureName, szFileName); // 서브셋이 여러개인 경우에는 저장이 애매함..

			// Diffuse
			if(FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->ppTextures[i])))
			{
				MSG_BOX(L"Dynamic_Mesh Subset Texture Create Failed - CHierarchyLoader::CreateMeshContainer");
				return E_FAIL;
			}

			// _를 찾아서 그 이름 앞까지만 저장.
			TCHAR* pNextToken = nullptr;
			TCHAR* pSliceStr = nullptr;
			_tchar ch[2] = L"";
			lstrcpy(ch, L"_");

			pSliceStr = _tcstok_s(szFileName, ch, &pNextToken);

			// Normal
			lstrcpy(szNormalTex, pSliceStr);
			lstrcat(szNormalTex, L"_n.tga");

			lstrcpy(szFullPath, m_pFilePath);
			lstrcat(szFullPath, szNormalTex);

			// 만약 노말이 있으면 저장하고 아니면 NULL로 채워야함.
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->ppNormalTextures[i])))
			{
				pNewMeshContainer->ppNormalTextures[i] = nullptr;
			}

			// Spec
			lstrcpy(szSpecTex, pSliceStr);
			lstrcat(szSpecTex, L"_s.tga");

			lstrcpy(szFullPath, m_pFilePath);
			lstrcat(szFullPath, szSpecTex);

			// 만약 스펙큘러 있으면 저장하고 아니면 NULL로 채워야함.
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->ppSpecTextures[i])))
			{
				pNewMeshContainer->ppSpecTextures[i] = nullptr;
			}

			// Emissive
			lstrcpy(szSpecTex, pSliceStr);
			lstrcat(szSpecTex, L"_e.tga");

			lstrcpy(szFullPath, m_pFilePath);
			lstrcat(szFullPath, szSpecTex);

			// 만약 Emissive 있으면 저장하고 아니면 NULL로 채워야함.
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->ppEmissiveTextures[i])))
			{
				pNewMeshContainer->ppEmissiveTextures[i] = nullptr;
			}
		}
	}
	else
	{
		pNewMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Power = 1.0f;

		pNewMeshContainer->ppTextures[0] = nullptr;
		pNewMeshContainer->ppNormalTextures[0] = nullptr;
		pNewMeshContainer->ppSpecTextures[0] = nullptr;
		pNewMeshContainer->ppEmissiveTextures[0] = nullptr;
	}

	if(nullptr != pSkinInfo)
	{
		/*pNewMeshContainer->MeshData.pMesh->CloneMeshFVF(pNewMeshContainer->MeshData.pMesh->GetOptions()
			, pNewMeshContainer->MeshData.pMesh->GetFVF(), m_pGraphicDev, &pNewMeshContainer->pOriMesh);*/

		pNewMeshContainer->pSkinInfo = pSkinInfo;
		pNewMeshContainer->pSkinInfo->AddRef();
		pNewMeshContainer->dwNumBone = pSkinInfo->GetNumBones();

		//////////////////////////////////////////////////////////////////////////
		// 하드웨어 스키닝
		DWORD maxVertInflunences = 0;
		DWORD numBoneComboEntries = 0;
		//LPD3DXMESH
		pSkinInfo->ConvertToIndexedBlendedMesh(pNewMeshContainer->MeshData.pMesh
			, D3DXMESH_MANAGED | D3DXMESH_WRITEONLY,
			pNewMeshContainer->dwNumBone, nullptr, nullptr, nullptr, nullptr, &maxVertInflunences, &numBoneComboEntries
			, &pNewMeshContainer->pBoneCombinationBuf
			, &pNewMeshContainer->MeshData.pMesh);
		
		pNewMeshContainer->dwMartixPaletteCnt = maxVertInflunences;
		//////////////////////////////////////////////////////////////////////////
		
		pNewMeshContainer->ppCombinedTransformationMatrices = new _matrix*[pNewMeshContainer->dwNumBone];
		pNewMeshContainer->pOffsetMatrices = new _matrix[pNewMeshContainer->dwNumBone];
		pNewMeshContainer->pRenderingMatrices = new _matrix[pNewMeshContainer->dwNumBone];

		for (_ulong i = 0; i < pNewMeshContainer->dwNumBone; ++i)		
			pNewMeshContainer->pOffsetMatrices[i] = *pNewMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);		
	}
	
	*ppNewMeshContainer = pNewMeshContainer;		

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::DestroyFrame(THIS_ LPD3DXFRAME pFrameToFree)
{
	if(nullptr == pFrameToFree)
		return S_OK;

	Engine::Safe_Delete_Array(pFrameToFree->Name);
	
	if (nullptr != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if(nullptr != pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if(nullptr != pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	Engine::Safe_Delete(pFrameToFree);

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::DestroyMeshContainer(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED*			pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		Engine::Safe_Release(pMeshContainer->ppTextures[i]);

		if(nullptr != pMeshContainer->ppNormalTextures[i])
			Engine::Safe_Release(pMeshContainer->ppNormalTextures[i]);

		if (nullptr != pMeshContainer->ppSpecTextures[i])
			Engine::Safe_Release(pMeshContainer->ppSpecTextures[i]);

		if (nullptr != pMeshContainer->ppEmissiveTextures[i])
			Engine::Safe_Release(pMeshContainer->ppEmissiveTextures[i]);
	}
	Engine::Safe_Delete_Array(pMeshContainer->ppTextures);
	Engine::Safe_Delete_Array(pMeshContainer->ppNormalTextures);
	Engine::Safe_Delete_Array(pMeshContainer->ppSpecTextures);
	Engine::Safe_Delete_Array(pMeshContainer->ppEmissiveTextures);
	
	Engine::Safe_Delete_Array(pMeshContainer->ppCombinedTransformationMatrices);
	Engine::Safe_Delete_Array(pMeshContainer->pRenderingMatrices);

	Engine::Safe_Delete_Array(pMeshContainer->Name);
	Engine::Safe_Delete_Array(pMeshContainer->pAdjacency);
	Engine::Safe_Delete_Array(pMeshContainer->pMaterials);
	Engine::Safe_Delete_Array(pMeshContainer->pOffsetMatrices);
	
	Engine::Safe_Release(pMeshContainer->pBoneCombinationBuf);	// 하드웨어 스키닝 변수 제거

	Engine::Safe_Release(pMeshContainer->MeshData.pMesh);	
	Engine::Safe_Release(pMeshContainer->pSkinInfo);
	Engine::Safe_Release(pMeshContainer->pOriMesh);

	Engine::Safe_Delete(pMeshContainer);

	return S_OK;
}

void Engine::CHierarchyLoader::Allocate_Name(char** ppSrcName, const char* pDestName)
{
	_uint		iLength = strlen(pDestName);

	*ppSrcName = new char[iLength + 1];

	strcpy_s(*ppSrcName, iLength + 1, pDestName);
}

CHierarchyLoader* Engine::CHierarchyLoader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath)
{
	CHierarchyLoader*		pInstance = new CHierarchyLoader(pGraphicDev, pFilePath);

	if(FAILED(pInstance->Ready_HierarchyLoader()))
	{
		MSG_BOX(L"CHierarchyLoader Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong Engine::CHierarchyLoader::Release(void)
{
	if(0 == m_dwRefCnt)
	{
		Engine::Safe_Release(m_pGraphicDev);		

		delete this;

		return 0;
	}

	return m_dwRefCnt--;	
}