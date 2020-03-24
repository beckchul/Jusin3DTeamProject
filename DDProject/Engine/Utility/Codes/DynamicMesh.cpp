#include "DynamicMesh.h"
#include "HierarchyLoader.h"
#include "AnimationCtrl.h"

USING(Engine)

Engine::CDynamicMesh::CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev)
: CMesh(pGraphicDev)
, m_pLoader(nullptr)
, m_pRootFrame(nullptr)
{
	ZeroMemory(m_pAnimationCtrl, sizeof(CAnimationCtrl*) * ANICTRL_END);
}

Engine::CDynamicMesh::CDynamicMesh(const CDynamicMesh& rhs)
: CMesh(rhs)
, m_pLoader(rhs.m_pLoader)
, m_pRootFrame(rhs.m_pRootFrame)
, m_MeshContainerList(rhs.m_MeshContainerList)
, m_matParent(rhs.m_matParent)
{
	m_pLoader->AddRef();
	m_pAnimationCtrl[ANICTRL_HIGHER] = CAnimationCtrl::Create(*rhs.m_pAnimationCtrl[ANICTRL_HIGHER]);
	m_pAnimationCtrl[ANICTRL_LOWER] = CAnimationCtrl::Create(*rhs.m_pAnimationCtrl[ANICTRL_LOWER]);
}

Engine::CDynamicMesh::~CDynamicMesh(void)
{

}

_matrix* Engine::CDynamicMesh::Get_FrameMatrix(const char* pFrameName) const
{
	D3DXFRAME_DERIVED* pFindFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName);

	if(nullptr == pFindFrame)
		return nullptr;

	return &pFindFrame->CombinedTransformationMatrix;
}

_double CDynamicMesh::GetPeriod(void)
{
	return m_pAnimationCtrl[ANICTRL_HIGHER]->GetPeriod();
}

_double CDynamicMesh::GetTrackPosition(void)
{
	return m_pAnimationCtrl[ANICTRL_HIGHER]->GetTrackPos();
}

void CDynamicMesh::Set_TrackPosition(const _int& iIndex, const _float & fTimeDelta)
{
	m_pAnimationCtrl[ANICTRL_HIGHER]->Set_TrackPosition(iIndex, fTimeDelta);
}

HRESULT Engine::CDynamicMesh::Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName)
{
	_tchar			szFullPath[128] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	m_pLoader = CHierarchyLoader::Create(m_pGraphicDev, pFilePath);
	if (nullptr == m_pLoader)
		return E_FAIL;

	m_pAnimationCtrl[ANICTRL_HIGHER] = CAnimationCtrl::Create(m_pGraphicDev);
	if (nullptr == m_pAnimationCtrl)
		return E_FAIL;

	HRESULT hr = NOERROR;

	hr = D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, m_pGraphicDev, m_pLoader, nullptr, &m_pRootFrame, m_pAnimationCtrl[ANICTRL_HIGHER]->Get_AnimationCtrl());
	if (FAILED(hr))
		return E_FAIL;

	//hr = (D3DXCreateTexture(m_pGraphicDev, 32, 32, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pPaletteTexture));
	//if (FAILED(hr))
	//	return E_FAIL;

	D3DXMatrixRotationY(&m_matParent, D3DXToRadian(180.0f));
	Update_CombinedMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, &m_matParent);
	SetUp_MatrixPointer((D3DXFRAME_DERIVED*)m_pRootFrame);

	m_pAnimationCtrl[ANICTRL_LOWER] = CAnimationCtrl::Create(*m_pAnimationCtrl[ANICTRL_HIGHER]);

	return S_OK;
}


// ������ ������ �ִ� CombinedTransformationMatrix�� ���� ä����.
void Engine::CDynamicMesh::Update_CombinedMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pMatrix)
{
	pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix * *pMatrix;

	if (nullptr != pFrame->pFrameSibling)
		Update_CombinedMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, pMatrix);

	if (nullptr != pFrame->pFrameFirstChild)
		Update_CombinedMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
}

void CDynamicMesh::Update_CombinedMatrices(D3DXFRAME_DERIVED * pFrame, const _matrix * pMatrix, const char** pBoneName)
{

	pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix * *pMatrix;

	if (nullptr != pFrame->pFrameSibling)
	{
		if (strcmp(pBoneName[0], pFrame->pFrameSibling->Name) &&
			strcmp(pBoneName[1], pFrame->pFrameSibling->Name) &&
			strcmp(pBoneName[2], pFrame->pFrameSibling->Name))
			Update_CombinedMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, pMatrix, pBoneName);
	}
	if (nullptr != pFrame->pFrameFirstChild)
	{
		if (!strcmp(pBoneName[0], pFrame->pFrameFirstChild->Name) ||
			!strcmp(pBoneName[1], pFrame->pFrameFirstChild->Name) ||
			!strcmp(pBoneName[2], pFrame->pFrameFirstChild->Name))
			return;
		Update_CombinedMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix, pBoneName);
	}	
}

void Engine::CDynamicMesh::SetUp_MatrixPointer(D3DXFRAME_DERIVED* pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED*		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		for (_ulong i = 0; i < pMeshContainer->dwNumBone; ++i)
		{
			const char* pFrameName = pMeshContainer->pSkinInfo->GetBoneName(i);

			D3DXFRAME_DERIVED* pFrameFind = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName);

			pMeshContainer->ppCombinedTransformationMatrices[i] = &pFrameFind->CombinedTransformationMatrix;
		}
		m_MeshContainerList.push_back(pMeshContainer);
	}

	if (nullptr != pFrame->pFrameSibling)
		SetUp_MatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameSibling);

	if (nullptr != pFrame->pFrameFirstChild)
		SetUp_MatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild);
}

void Engine::CDynamicMesh::Render_Mesh(void)
{
	MESHCONTAINERLIST::iterator	iter = m_MeshContainerList.begin();
	MESHCONTAINERLIST::iterator	iter_end = m_MeshContainerList.end();

	for (; iter != iter_end; ++iter)
	{
		for (_ulong i = 0; i < (*iter)->dwNumBone; ++i)
			(*iter)->pRenderingMatrices[i] = (*iter)->pOffsetMatrices[i] * *(*iter)->ppCombinedTransformationMatrices[i];

		void*		pSrcVertices = nullptr, *pDestVertices = nullptr;

		(*iter)->pOriMesh->LockVertexBuffer(0, &pSrcVertices);
		(*iter)->MeshData.pMesh->LockVertexBuffer(0, &pDestVertices);

		(*iter)->pSkinInfo->UpdateSkinnedMesh((*iter)->pRenderingMatrices, nullptr, pSrcVertices, pDestVertices); // ���� �޽������̳ʸ� �׷������� ��ĸ�ŭ ��ȯ���ش�.

		(*iter)->pOriMesh->UnlockVertexBuffer();
		(*iter)->MeshData.pMesh->UnlockVertexBuffer();

		for (_ulong i = 0; i < (*iter)->NumMaterials; ++i)
		{
			m_pGraphicDev->SetTexture(0, (*iter)->ppTextures[i]);
			(*iter)->MeshData.pMesh->DrawSubset(i);
		}
	}
}

void Engine::CDynamicMesh::Render_Mesh(LPD3DXEFFECT pEffect, _uint uPassIdx)
{
	MESHCONTAINERLIST::iterator	iter = m_MeshContainerList.begin();
	MESHCONTAINERLIST::iterator	iter_end = m_MeshContainerList.end();

	pEffect->SetFloat("g_fFar", PLANE_FAR);

	// ���� �н����� ����س��´�.
	_uint passTemp = uPassIdx;

	bool bShadowMap = false;

	if (uPassIdx == 100)
		bShadowMap = true;

	for (; iter != iter_end; ++iter)
	{
		LPD3DXBONECOMBINATION pBoneComb = (LPD3DXBONECOMBINATION)(*iter)->pBoneCombinationBuf->GetBufferPointer();

		for (_ulong iPaletteEntry = 0; iPaletteEntry < (*iter)->dwNumBone; ++iPaletteEntry)
		{
			(*iter)->pRenderingMatrices[iPaletteEntry] = (*iter)->pOffsetMatrices[iPaletteEntry] * *(*iter)->ppCombinedTransformationMatrices[iPaletteEntry];
		}

		pEffect->SetMatrixArray("MatrixPalette", (*iter)->pRenderingMatrices, (*iter)->dwNumBone);
		pEffect->SetInt("numBoneInfluences", (*iter)->dwMartixPaletteCnt);
		
		if (bShadowMap)
		{
			// �׸��ڸ��� �׸��� �÷��̾ �׸��Ŵ�.

			for (_ulong i = 0; i < (*iter)->NumMaterials; ++i)
			{
				if (!lstrcmp(L"lilMageDecoloredChannels_d.tga", (*iter)->szTextureName) ||
					!lstrcmp(L"monk_d.tga", (*iter)->szTextureName) ||
					!lstrcmp(L"Huntress_d.tga", (*iter)->szTextureName))
				{
					pEffect->CommitChanges();

					pEffect->BeginPass(1);

					(*iter)->MeshData.pMesh->DrawSubset(i);

					pEffect->EndPass();
				}
			}
		}
		else
		{
			//int mtrlIndex = pBoneComb->AttribId;

			for (_ulong i = 0; i < (*iter)->NumMaterials; ++i)
			{
				if(nullptr != (*iter)->ppTextures[i])
					pEffect->SetTexture("g_BaseTexture", (*iter)->ppTextures[i]);

				if(nullptr != (*iter)->ppNormalTextures[i])
					pEffect->SetTexture("g_NormalTexture", (*iter)->ppNormalTextures[i]);

				if(nullptr != (*iter)->ppSpecTextures[i])
					pEffect->SetTexture("g_SpecularIntensityTexture", (*iter)->ppSpecTextures[i]);

				if (nullptr != (*iter)->ppEmissiveTextures[i])
				{
					pEffect->SetTexture("g_EmissiveTexture", (*iter)->ppEmissiveTextures[i]);
					pEffect->SetBool("g_bEmissiveExist", true);
				}
				else
				{
					pEffect->SetBool("g_bEmissiveExist", false);
				}
				// 1. �븻, ����ŧ�� �ִ°��.
				if (nullptr != (*iter)->ppNormalTextures[i] && nullptr != (*iter)->ppSpecTextures[i])
				{
					uPassIdx = 0;
				}
				// 2. �븻�� �ִ°��.
				else if (nullptr != (*iter)->ppNormalTextures[i])
				{
					uPassIdx = 1;
				}
				// 3. �ƿ� ���� ���.
				else
				{
					uPassIdx = 2;
				}
				// ����ũ���̳� Emissive�� ���������� ��������.


				// ������ ���� Pass�� ������ ���� ó���ϰ� �������� ���⼭ �ٽ� �н��� �����Ѵ�.

				// Ÿ���� �ٸ� �н��� ���� ������ ������ �ؾ������� �̰� �����Ǿ������� �ȵȴ�.
				// ���� Ÿ���� ��쿡�� ������ ���� �н����� �̿��� ó���ؾ��Ѵ�.
				if (!lstrcmp(L"missleTower_d.tga", (*iter)->szTextureName) ||
					!lstrcmp(L"fireTower_d.tga", (*iter)->szTextureName) ||
					!lstrcmp(L"lightningTower_d.tga", (*iter)->szTextureName) ||
					!lstrcmp(L"DeadlyStriker_d.tga", (*iter)->szTextureName) ||
					!lstrcmp(L"MageBlockade_d.tga", (*iter)->szTextureName))
				{
					uPassIdx = passTemp;

					pEffect->SetFloat("g_fBrightNess", 0.4f);
				}
				else
				{
					pEffect->SetFloat("g_fBrightNess", 0.f);
				}


				pEffect->CommitChanges();

				pEffect->BeginPass(uPassIdx);

				(*iter)->MeshData.pMesh->DrawSubset(i);

				pEffect->EndPass();
			}
			// �н� ������ �� ����. -> �ٸ� ��ü�� ���� �Ȱ���
			pEffect->SetVector("g_fHitColor", &_vec4(0.f, 0.f, 0.f, 0.f));
		}
	}
}

void Engine::CDynamicMesh::Set_AnimationSet(Engine::ANI_INFO tAniInfo)
{
	m_pAnimationCtrl[ANICTRL_HIGHER]->Set_AnimationSet(tAniInfo);
}

void CDynamicMesh::Set_LowAnimationSet(Engine::ANI_INFO _tLowAniInfo)
{
	m_pAnimationCtrl[ANICTRL_LOWER]->Set_AnimationSet(_tLowAniInfo);
}

void CDynamicMesh::Play_AnimationSet(const _float& fTimeDelta, _float fCamAngle, const char** pLowBoneName, const char** pHighBoneName)
{
	m_pAnimationCtrl[ANICTRL_LOWER]->Play_AnimationSet(fTimeDelta);

	Update_CombinedMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, &m_matParent, pHighBoneName);

	m_pAnimationCtrl[ANICTRL_HIGHER]->Play_AnimationSet(fTimeDelta);
	_matrix matRotX, matRotY;
	D3DXMatrixRotationX(&matRotX, D3DXToRadian(-fCamAngle));
	D3DXMatrixRotationY(&matRotY, D3DXToRadian(180.0f));
	matRotX *= matRotY;
	Update_CombinedMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, &matRotX, pLowBoneName);
}

void CDynamicMesh::Play_AnimationSet(const _float& fTimeDelta)
{
	m_pAnimationCtrl[ANICTRL_HIGHER]->Play_AnimationSet(fTimeDelta);
	Update_CombinedMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, &m_matParent);
}

void Engine::CDynamicMesh::Compute_MinMax(_vec3* pMin, _vec3* pMax, const _uint& iContainerIdx)
{
	D3DVERTEXELEMENT9			Element[MAX_FVF_DECL_SIZE];
	ZeroMemory(Element, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);

	MESHCONTAINERLIST::iterator	iter = m_MeshContainerList.begin();

	for (_uint i = 0; i < iContainerIdx; ++i)
		++iter;

	(*iter)->MeshData.pMesh->GetDeclaration(Element);
	
	WORD	byOffset = 0;

	for (_ulong i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if(D3DDECLUSAGE_POSITION == Element[i].Usage)
		{
			byOffset = Element[i].Offset;
			break;
		}
	}

	_byte*		pVertex = nullptr;

	(*iter)->MeshData.pMesh->LockVertexBuffer(0, (void**)&pVertex);

	_ulong dwFVF = (*iter)->MeshData.pMesh->GetFVF();

	D3DXComputeBoundingBox((_vec3*)(pVertex + byOffset), (*iter)->MeshData.pMesh->GetNumVertices(), D3DXGetFVFVertexSize(dwFVF), pMin, pMax);

	(*iter)->MeshData.pMesh->UnlockVertexBuffer();
}

CComponent* Engine::CDynamicMesh::Clone(void)
{
	return new CDynamicMesh(*this);
}

CDynamicMesh* Engine::CDynamicMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName)
{
	CDynamicMesh*		pInstance = new CDynamicMesh(pGraphicDev);

	if(FAILED(pInstance->Ready_Meshes(pFilePath, pFileName)))
	{
		MSG_BOX(L"CDynamicMesh Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::CDynamicMesh::Free(void)
{
	Engine::CMesh::Free();

	if(false == m_isClone) // ������ü�� ��쿡��.
	{
		m_pLoader->DestroyFrame(m_pRootFrame);
	}

	for(size_t i = 0; i < ANICTRL_END; ++i)
		Engine::Safe_Release(m_pAnimationCtrl[i]);

	m_MeshContainerList.clear();

	Engine::Safe_Release(m_pLoader);
}

