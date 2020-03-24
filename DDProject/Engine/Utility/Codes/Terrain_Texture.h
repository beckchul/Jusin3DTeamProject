#ifndef Terrain_Texture_h__
#define Terrain_Texture_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrain_Texture : public CVIBuffer
{
private:
	explicit CTerrain_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrain_Texture(const CTerrain_Texture& rhs);
	virtual ~CTerrain_Texture(void);
public:
	const _vec3* Get_VtxPos(void) const {
		return m_pPosition;
	}
	_ulong Get_VtxCntX(void) const {
		return m_ih.biWidth;
	}
	_ulong Get_VtxCntZ(void) const {
		return m_ih.biHeight;
	}
public:
	void Copy_Indices(Engine::INDEX32* pIndices, const _ulong& dwTriCnt);
public:
	HRESULT Ready_Buffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxInterval);
public:
	virtual void Render_Buffer(void);
private:
	HANDLE					m_hFile;
	BITMAPFILEHEADER		m_fh;
	BITMAPINFOHEADER		m_ih;
	_vec3*					m_pPosition;
	_bool					m_isClone;
public:
	virtual CComponent* Clone(void);
	static CTerrain_Texture* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxInterval);
private:
	virtual void Free(void);
};


END

#endif // Terrain_Texture_h__
