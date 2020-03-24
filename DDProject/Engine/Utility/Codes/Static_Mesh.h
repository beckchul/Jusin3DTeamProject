#ifndef StaticMesh_h__
#define StaticMesh_h__

#include "Mesh.h"

BEGIN(Engine)

class CCubeColor;

class ENGINE_DLL CStaticMesh : public CMesh
{
private:
	explicit CStaticMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CStaticMesh(const CStaticMesh& rhs);
	virtual ~CStaticMesh(void);
	// Getter
public:
	LPD3DXMESH Get_Mesh(void) {	return m_pMesh;}

public:
	HRESULT Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	void Render_Mesh();
	void Render_Mesh_Effect(LPD3DXEFFECT pEffect, _uint uPassIdx = 0);
	void Render_Mesh(LPD3DXEFFECT pEffect, _uint uPassIdx = 0);
public:
	void Compute_MinMax(_vec3* pMin, _vec3* pMax);
private:
	LPD3DXMESH					m_pOriMesh;
	LPD3DXMESH					m_pMesh;
	LPD3DXBUFFER				m_pAdjacency;
	LPD3DXBUFFER				m_pSubSetBuffer;
	D3DXMATERIAL*				m_pSubSets;
	_ulong						m_dwSubSetCnt;
	LPDIRECT3DTEXTURE9*			m_ppTextures = nullptr;
	LPDIRECT3DTEXTURE9*			m_ppNormalTextures = nullptr;
	LPDIRECT3DTEXTURE9*			m_ppSpecTextures = nullptr;
	LPDIRECT3DTEXTURE9*			m_ppEmissiveTextures = nullptr;
	LPDIRECT3DTEXTURE9*			m_ppMaskTextures = nullptr;

	LPDIRECT3DCUBETEXTURE9*		m_pCubeTex = nullptr;

	// 어느 서브셋에 종속되어 있기 애매한 텍스처는 전역적으로 미리 주소를 받아놓고 사용한다.
	LPDIRECT3DTEXTURE9*			m_pTexLavaNoiseNormal = nullptr; // lavaNoise_n
	LPDIRECT3DTEXTURE9*			m_pTexLavaNoise = nullptr; // lavaNoise_o
	LPDIRECT3DTEXTURE9*			m_pTexLaveA = nullptr; // lave_a
	LPDIRECT3DTEXTURE9*			m_pTexChessBoard = nullptr; // ChessBoard
	LPDIRECT3DTEXTURE9*			m_pTexCloudMerge = nullptr; // CloudMerge
	LPDIRECT3DTEXTURE9*			m_pTexRainNormal = nullptr; // Rain Noraml
	LPDIRECT3DTEXTURE9*			m_pTexCloud = nullptr; // clouds

private:
	///bool		m_bCubeLoad = false;

private:
	_bool Find_Alpha(const char* pFileName);

public:
	static CStaticMesh* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);


public:
	virtual CComponent* Clone(void);
public:
	virtual void Free(void);

};

END

#endif // StaticMesh_h__
