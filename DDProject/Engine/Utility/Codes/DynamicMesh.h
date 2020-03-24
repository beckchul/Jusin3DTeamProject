#ifndef DynamicMesh_h__
#define DynamicMesh_h__

#include "Mesh.h"
#include "AnimationCtrl.h"

BEGIN(Engine)

class CHierarchyLoader;

class ENGINE_DLL CDynamicMesh : public CMesh
{
public:
	enum AniCtrlEnum { ANICTRL_HIGHER, ANICTRL_LOWER, ANICTRL_END };
private:
	explicit CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicMesh(const CDynamicMesh& rhs);
	virtual ~CDynamicMesh(void);
public:
	_matrix* Get_FrameMatrix(const char* pFrameName) const;
	_double CDynamicMesh::GetPeriod(void);
	_double GetTrackPosition(void);
public:
	void Set_TrackPosition(const _int& iIndex, const _float& fTimeDelta);
public:
	HRESULT Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	void Update_CombinedMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pMatrix);
	void Update_CombinedMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pMatrix, const char** pBoneName);
	void SetUp_MatrixPointer(D3DXFRAME_DERIVED* pFrame);
	void Render_Mesh(void);	
	void Render_Mesh(LPD3DXEFFECT pEffect, _uint uPassIdx = 0);
	void Set_AnimationSet(ANI_INFO tAniInfo);
	void Set_LowAnimationSet(ANI_INFO _tLowAniInfo);
	void Play_AnimationSet(const _float& fTimeDelta);
	void Play_AnimationSet(const _float& fTimeDelta, _float fCamAngle, const char** pLowBoneName, const char** pHighBoneName);
public:
	void Compute_MinMax(_vec3* pMin, _vec3* pMax, const _uint& iContainerIdx);
private:
	CHierarchyLoader*							m_pLoader;
	CAnimationCtrl*								m_pAnimationCtrl[ANICTRL_END];
	LPD3DXFRAME									m_pRootFrame;
	_matrix										m_matParent;
	//LPDIRECT3DTEXTURE9							m_pPaletteTexture;
private:
	list<D3DXMESHCONTAINER_DERIVED*>			m_MeshContainerList;
	typedef list<D3DXMESHCONTAINER_DERIVED*>	MESHCONTAINERLIST;
public:
	virtual CComponent* Clone(void);
	static CDynamicMesh* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);
private:
	virtual void Free(void);

};

END

#endif // DynamicMesh_h__
