#include "Static_Mesh.h"
#include "CubeColor.h"
#include "DataManager.h"
#include "TextureManager.h"

USING(Engine)

Engine::CStaticMesh::CStaticMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CMesh(pGraphicDev)
{

}

Engine::CStaticMesh::CStaticMesh(const CStaticMesh& rhs)
	: Engine::CMesh(rhs)
	, m_pAdjacency(rhs.m_pAdjacency)
	, m_pSubSetBuffer(rhs.m_pSubSetBuffer)
	, m_pSubSets(rhs.m_pSubSets)
	, m_dwSubSetCnt(rhs.m_dwSubSetCnt)
	, m_pMesh(rhs.m_pMesh)
	, m_pOriMesh(rhs.m_pOriMesh)
	, m_ppTextures(rhs.m_ppTextures)
	, m_ppNormalTextures(rhs.m_ppNormalTextures)
	, m_ppSpecTextures(rhs.m_ppSpecTextures)
	, m_ppEmissiveTextures(rhs.m_ppEmissiveTextures)
	, m_ppMaskTextures(rhs.m_ppMaskTextures)
{
	/*m_ppTextures = new LPDIRECT3DTEXTURE9[rhs.m_dwSubSetCnt];*/

	for (_ulong i = 0; i < rhs.m_dwSubSetCnt; ++i)
	{
		m_ppTextures[i] = rhs.m_ppTextures[i];
		m_ppTextures[i]->AddRef();

		m_ppNormalTextures[i] = rhs.m_ppNormalTextures[i];

		if (nullptr != m_ppNormalTextures[i])
			m_ppNormalTextures[i]->AddRef();

		m_ppSpecTextures[i] = rhs.m_ppSpecTextures[i];

		if (nullptr != m_ppSpecTextures[i])
			m_ppSpecTextures[i]->AddRef();

		m_ppEmissiveTextures[i] = rhs.m_ppEmissiveTextures[i];

		if (nullptr != m_ppEmissiveTextures[i])
			m_ppEmissiveTextures[i]->AddRef();

		m_ppMaskTextures[i] = rhs.m_ppMaskTextures[i];

		if (nullptr != m_ppMaskTextures[i])
			m_ppMaskTextures[i]->AddRef();

	}

	m_pAdjacency->AddRef();
	m_pSubSetBuffer->AddRef();
	m_pOriMesh->AddRef();
	m_pMesh->AddRef();

}

Engine::CStaticMesh::~CStaticMesh(void)
{

}

HRESULT Engine::CStaticMesh::Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName)
{
	

	_tchar			szFullPath[128] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	_ulong	dwSubSetCnt = 0;


	if (FAILED(D3DXLoadMeshFromX(szFullPath, D3DXMESH_MANAGED, m_pGraphicDev, &m_pAdjacency, &m_pSubSetBuffer, nullptr, &dwSubSetCnt, &m_pOriMesh)))
		return E_FAIL;

	// 메시를 구성하고 있는 정점의 fvf정보를 리턴한다.
	_ulong dwFVF = m_pOriMesh->GetFVF();

	if (dwFVF & D3DFVF_NORMAL)
	{
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF, m_pGraphicDev, &m_pMesh);
	}
	else
	{
		// 내가 지정한 fvf정보로 메시를 복제하겠다.
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pGraphicDev, &m_pMesh);

		// 노멀을 계산해준다.
		D3DXComputeNormals(m_pMesh, (_ulong*)m_pAdjacency->GetBufferPointer());
	}

	// D3DXBuffer가 담고있는 정보의 가장 앞 주소를 리턴
	m_pSubSets = (D3DXMATERIAL*)m_pSubSetBuffer->GetBufferPointer();

	m_dwSubSetCnt = dwSubSetCnt == 0 ? 1 : dwSubSetCnt;

	m_ppTextures = new LPDIRECT3DTEXTURE9[m_dwSubSetCnt]();
	m_ppNormalTextures = new LPDIRECT3DTEXTURE9[m_dwSubSetCnt]();
	m_ppSpecTextures = new LPDIRECT3DTEXTURE9[m_dwSubSetCnt]();
	m_ppEmissiveTextures = new LPDIRECT3DTEXTURE9[m_dwSubSetCnt]();
	m_ppMaskTextures = new LPDIRECT3DTEXTURE9[m_dwSubSetCnt]();

	if (0 != dwSubSetCnt)
	{
		_tchar szNormalTex[128] = L"";
		_tchar szSpecTex[128] = L"";

		for (_ulong i = 0; i < m_dwSubSetCnt; ++i)
		{
			_tchar szFileName[128] = L"";

			lstrcpy(szFullPath, pFilePath);

			// 서브셋의 텍스처파일 이름을 유니코드 형태로 변환.
			MultiByteToWideChar(CP_ACP, 0, m_pSubSets[i].pTextureFilename, strlen(m_pSubSets[i].pTextureFilename)
				, szFileName, 128);

			lstrcat(szFullPath, szFileName);

			// Diffuse
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppTextures[i])))
			{
				MSG_BOX(L"Static_Mesh Subset Texture Create Failed - CStatic_Mesh::Ready_Meshes");
				return E_FAIL;
			}

			// _를 찾아서 그 이름 앞까지만 저장을 따로 해야함.
			TCHAR* pNextToken = nullptr;
			TCHAR* pSliceStr = nullptr;

			_tchar ch[2] = L"";
			lstrcpy(ch, L"_");

			pSliceStr = _tcstok_s(szFileName, ch, &pNextToken);

			//// 크리스탈 큐브맵 텍스처
			//if (!m_bCubeLoad && !lstrcmp(pSliceStr, L"Core01"))
			//{
			//	m_bCubeLoad = true;

			//	lstrcpy(szFullPath, pFilePath);
			//	lstrcat(szFullPath, L"CubeMap.dds");
			//	if (FAILED(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFullPath, &m_pCubeTex)))
			//	{
			//		MSG_BOX(L"Static_Mesh Subset Texture Create Failed - CStatic_Mesh::Ready_Meshes");
			//		return E_FAIL;
			//	}
			//}

			// Normal
			lstrcpy(szNormalTex, pSliceStr);
			lstrcat(szNormalTex, L"_n.tga");

			lstrcpy(szFullPath, pFilePath);
			lstrcat(szFullPath, szNormalTex);

			// 만약 노말이 있으면 저장하고 아니면 NULL로 채워야함.
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppNormalTextures[i])))
			{
				m_ppNormalTextures[i] = nullptr;
			}

			// Spec
			lstrcpy(szSpecTex, pSliceStr);
			lstrcat(szSpecTex, L"_s.tga");

			lstrcpy(szFullPath, pFilePath);
			lstrcat(szFullPath, szSpecTex);

			// 만약 스펙큘러 있으면 저장하고 아니면 NULL로 채워야함.
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppSpecTextures[i])))
			{
				m_ppSpecTextures[i] = nullptr;
			}

			// Emissive
			lstrcpy(szSpecTex, pSliceStr);
			lstrcat(szSpecTex, L"_e.tga");

			lstrcpy(szFullPath, pFilePath);
			lstrcat(szFullPath, szSpecTex);

			// 만약 Emissive 있으면 저장하고 아니면 NULL로 채워야함.
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppEmissiveTextures[i])))
			{
				m_ppEmissiveTextures[i] = nullptr;
			}

			// Mask
			lstrcpy(szSpecTex, pSliceStr);
			lstrcat(szSpecTex, L"_m.tga");

			lstrcpy(szFullPath, pFilePath);
			lstrcat(szFullPath, szSpecTex);

			// 만약 스펙큘러 있으면 저장하고 아니면 NULL로 채워야함.
			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppMaskTextures[i])))
			{
				m_ppMaskTextures[i] = nullptr;
			}
		}
	}

	else
	{
		m_pSubSets[0].MatD3D.Diffuse = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		m_pSubSets[0].MatD3D.Specular = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		m_pSubSets[0].MatD3D.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		m_pSubSets[0].MatD3D.Ambient = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		m_pSubSets[0].MatD3D.Power = 1.0f;

		m_ppTextures[0] = nullptr;
		m_ppNormalTextures[0] = nullptr;
		m_ppSpecTextures[0] = nullptr;
		m_ppEmissiveTextures[0] = nullptr;
		m_ppMaskTextures[0] = nullptr;
	}
	
	
	return S_OK;
}

void Engine::CStaticMesh::Render_Mesh()
{
	for (_ulong i = 0; i < m_dwSubSetCnt; ++i)
	{
		if (nullptr != m_ppTextures[i])
			m_pGraphicDev->SetTexture(0, m_ppTextures[i]);
		m_pMesh->DrawSubset(i);
	}
}


void Engine::CStaticMesh::Render_Mesh_Effect(LPD3DXEFFECT pEffect, _uint uPassIdx /*= 0*/)
{
	for (_ulong i = 0; i < m_dwSubSetCnt; ++i)
	{
		pEffect->SetTexture("g_DiffuseTexture", m_ppTextures[i]);
			
		pEffect->CommitChanges();

		pEffect->BeginPass(uPassIdx);

		m_pMesh->DrawSubset(i);

		pEffect->EndPass();
	}
}


void Engine::CStaticMesh::Render_Mesh(LPD3DXEFFECT pEffect, _uint uPassIdx)
{
	if (nullptr == m_pCubeTex)
		m_pCubeTex = CTextureManager::GetInstance()->Get_CubeTex();
	if (nullptr == m_pTexLavaNoise)
		m_pTexLavaNoise = CTextureManager::GetInstance()->Get_Texture(L"lavaNoise_o");
	if (nullptr == m_pTexLavaNoiseNormal)
		m_pTexLavaNoiseNormal = CTextureManager::GetInstance()->Get_Texture(L"lavaNoise_n");
	if (nullptr == m_pTexLaveA)
		m_pTexLaveA = CTextureManager::GetInstance()->Get_Texture(L"Lave_02_a");
	if(nullptr == m_pTexChessBoard)
		m_pTexChessBoard = CTextureManager::GetInstance()->Get_Texture(L"chessboard_d");
	if (nullptr == m_pTexCloudMerge)
		m_pTexCloudMerge = CTextureManager::GetInstance()->Get_Texture(L"cloudsMerge_1_2_3_o");
	if (nullptr == m_pTexRainNormal)
		m_pTexRainNormal = CTextureManager::GetInstance()->Get_Texture(L"Rain_2_n");
	if (nullptr == m_pTexCloud)
		m_pTexCloud = CTextureManager::GetInstance()->Get_Texture(L"clouds_a");


	pEffect->SetFloat("g_fFar", PLANE_FAR);

	bool bShadowMap = false;
	bool bDepthMap = false;

	if (uPassIdx == 100)
		bShadowMap = true;
	else if (uPassIdx == 200)
		bDepthMap = true; 

	_uint uPassTemp = uPassIdx;

	for (_ulong i = 0; i < m_dwSubSetCnt; ++i)
	{
		if (bShadowMap)
		{
			// 그림자맵을 그릴때 무기만 그릴거다.
			if (!strcmp("spear04_d.tga", m_pSubSets[i].pTextureFilename) || 
				!strcmp("Staff10_d.tga", m_pSubSets[i].pTextureFilename) ||
				!strcmp("crossbow01_d.tga", m_pSubSets[i].pTextureFilename))
			{
				pEffect->CommitChanges();

				pEffect->BeginPass(0);

				m_pMesh->DrawSubset(i);

				pEffect->EndPass();
			}
		}
		else if (bDepthMap)
		{
			// 깊이만 그림.
			pEffect->CommitChanges();

			pEffect->BeginPass(12);

			m_pMesh->DrawSubset(i);

			pEffect->EndPass();
		}
		else
		{
			///////////////////////////////////////////////////////////////////
			if(nullptr != m_ppTextures[i])
				pEffect->SetTexture("g_DiffuseTexture", m_ppTextures[i]);

			if(nullptr != m_ppNormalTextures[i])
				pEffect->SetTexture("g_NormalTexture", m_ppNormalTextures[i]);

			if(nullptr != m_ppSpecTextures[i])
				pEffect->SetTexture("g_SpecularIntensityTexture", m_ppSpecTextures[i]);

			if(nullptr != m_ppEmissiveTextures[i])
				pEffect->SetTexture("g_EmissiveTexture", m_ppEmissiveTextures[i]);

			if(nullptr != m_ppMaskTextures[i])
				pEffect->SetTexture("g_MaskTexture", m_ppMaskTextures[i]);

			// 1. 노말, 스펙큘러 있는경우.
			// 2. 노말만 있는경우.
			// 3. 아예 없는 경우.
			// 마스크맵이나 Emissive는 예외적으로 생각하자.
			if (nullptr != m_ppNormalTextures[i] && nullptr != m_ppSpecTextures[i])
			{
				uPassIdx = 0;
			}
			else if (nullptr != m_ppNormalTextures[i])
			{
				uPassIdx = 1;
			}
			else
			{
				uPassIdx = 2;
			}
			///////////////////////////////////////////////////////////////////


			// 위에서 정한 Pass와 별개로 따로 처리하고 싶은것은 여기서 다시 패스를 결정한다.

			if (!strcmp("CrystalLight_d.tga", m_pSubSets[i].pTextureFilename))
			{
				uPassIdx = 3;
			}
			else if (!strcmp("Core01_d.tga", m_pSubSets[i].pTextureFilename))
			{
				uPassIdx = 4;

				pEffect->SetTexture("EnvironmentMap_Tex", *m_pCubeTex);
				pEffect->SetVector("g_CamPosition", (_vec4*)&CDataManager::GetInstance()->Get_CamPoistion());
			}
			else if (!strcmp("Accessory01_d.tga", m_pSubSets[i].pTextureFilename))
			{
				uPassIdx = 5;
			}
			else if (!strcmp("ArcanePads_d.tga", m_pSubSets[i].pTextureFilename))
			{
				uPassIdx = 6;
			}
			else if (!strcmp("Lava_d.tga", m_pSubSets[i].pTextureFilename))
			{
				uPassIdx = 7;

				/*pEffect->SetBool("g_bNormalExist", true);
				pEffect->SetTexture("g_NormalTexture", m_ppNormalTextures[i]);*/

				pEffect->SetTexture("g_LavaNoiseNormalTexture", *m_pTexLavaNoiseNormal);
				pEffect->SetTexture("g_LavaNoiseTexture", *m_pTexLavaNoise);
				pEffect->SetTexture("g_LavaAlphaTexture", *m_pTexLaveA);

				// 여기서 쉐이더에 시간 값을 전달해야 한다.
				ULONGLONG tick = GetTickCount64(); // 컴퓨터가 부딩된 후부터 경과한 시간을 밀리초 단위로 반환

				// tick값 timemgr에서 반환하는 시간으로 바꿔야 함.
				pEffect->SetFloat("gTime", tick / 1000.f);
			}
			// 체크보드 따로 처리 임의의 텍스처를 내가 지정할꺼라.
			else if (!strcmp("chessboardBlack_d.tga", m_pSubSets[i].pTextureFilename) ||
				!strcmp("chessboardBlackBroken_d.tga", m_pSubSets[i].pTextureFilename) ||
				!strcmp("chessboardWhite_d.tga", m_pSubSets[i].pTextureFilename) ||
				!strcmp("chessboardWhiteBroken_d.tga", m_pSubSets[i].pTextureFilename))
			{
				uPassIdx = 2;

				pEffect->SetTexture("g_DiffuseTexture", *m_pTexChessBoard);

			}
			else if (!strcmp("manaTokenOutline_d.tga", m_pSubSets[i].pTextureFilename))
			{
				uPassIdx = 8;

				// 여기서 쉐이더에 시간 값을 전달해야 한다.
				ULONGLONG tick = GetTickCount64(); // 컴퓨터가 부딩된 후부터 경과한 시간을 밀리초 단위로 반환

				// tick값 timemgr에서 반환하는 시간으로 바꿔야 함.
				pEffect->SetFloat("gTime", tick / 1000.f);

				//pEffect->SetTexture("g_RainNormalTexture", *m_pTexLavaNoiseNormal);
				pEffect->SetTexture("g_CloudTexture", *m_pTexCloud);
			}
			else if (!strcmp("WallOrb_d.tga", m_pSubSets[i].pTextureFilename))
			{
				uPassIdx = 9;
			}
			else if (!strcmp("PaintingHuntress_d.tga", m_pSubSets[i].pTextureFilename))
			{
				uPassIdx = 10;

				pEffect->SetFloat("glowNess", 0.4f);
			}
			else if (!strcmp("JesterLamp_d.tga", m_pSubSets[i].pTextureFilename))
			{
				uPassIdx = 11;
			}
			else
			{
				//uPassIdx = uPassTemp;
				pEffect->SetFloat("gTime", 0.f);
			}

			pEffect->CommitChanges();

			pEffect->BeginPass(uPassIdx);

			m_pMesh->DrawSubset(i);

			pEffect->EndPass();
		}
		pEffect->SetFloat("glowNess", 1.f);
	}
}

void Engine::CStaticMesh::Compute_MinMax(_vec3* pMin, _vec3* pMax)
{
	D3DVERTEXELEMENT9		VtxFVF[MAX_FVF_DECL_SIZE];
	ZeroMemory(VtxFVF, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);
	m_pMesh->GetDeclaration(VtxFVF);
	
	long  dwOffset = -1;
	for (int i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (VtxFVF[i].Usage == D3DDECLUSAGE_POSITION)
		{
			dwOffset = VtxFVF[i].Offset;
			break;
		}
	}
	if (dwOffset < 0)
		return;

	void*	pVtxInfo = NULL;
	m_pMesh->LockVertexBuffer(0, &pVtxInfo);

	D3DXComputeBoundingBox((D3DXVECTOR3*)((BYTE*)pVtxInfo + dwOffset)
		, m_pMesh->GetNumVertices(), D3DXGetFVFVertexSize(m_pMesh->GetFVF())
		, pMin, pMax);

	m_pMesh->UnlockVertexBuffer();
}


_bool Engine::CStaticMesh::Find_Alpha(const char* pFileName)
{
	_uint iLength = strlen(pFileName);

	for (_uint i = 0; i < iLength + 1; ++i)
	{
		if (pFileName[i] == '.')
		{
			if (pFileName[i - 1] == 'A')
				return true;
		}
	}
	return false;
}

CStaticMesh* Engine::CStaticMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName)
{
	CStaticMesh*		pInstance = new CStaticMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Meshes(pFilePath, pFileName)))
	{
		MSG_BOX(L"CStaticMesh Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* Engine::CStaticMesh::Clone(void)
{
	return new CStaticMesh(*this);
}

void Engine::CStaticMesh::Free(void)
{
	Engine::CMesh::Free();

	for (_ulong i = 0; i < m_dwSubSetCnt; ++i)
	{
		Safe_Release(m_ppTextures[i]);
		
		if (nullptr != m_ppNormalTextures[i])
			Safe_Release(m_ppNormalTextures[i]);

		if (nullptr != m_ppSpecTextures[i])
			Safe_Release(m_ppSpecTextures[i]);

		if (nullptr != m_ppEmissiveTextures[i])
			Safe_Release(m_ppEmissiveTextures[i]);

		if (nullptr != m_ppMaskTextures[i])
			Safe_Release(m_ppMaskTextures[i]);
	}
	_ulong dwRefCnt = 0;
	dwRefCnt = Safe_Release(m_pOriMesh);
	dwRefCnt = Safe_Release(m_pMesh);
	Safe_Release(m_pAdjacency);
	Safe_Release(m_pSubSetBuffer);

	if (false == m_isClone)
	{
		Safe_Delete_Array(m_ppTextures);
		Safe_Delete_Array(m_ppNormalTextures);
		Safe_Delete_Array(m_ppSpecTextures);
		Safe_Delete_Array(m_ppEmissiveTextures);
		Safe_Delete_Array(m_ppMaskTextures);
	}
}

