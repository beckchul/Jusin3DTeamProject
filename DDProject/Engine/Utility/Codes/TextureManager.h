#ifndef TextureManager_h__
#define TextureManager_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CTextureManager : public CBase
{
	DECLARE_SINGLETON(CTextureManager)
public:

private:
	explicit CTextureManager(void);
	virtual ~CTextureManager(void);

public:
	LPDIRECT3DTEXTURE9* Get_Texture(const TCHAR* pTextureTag);
	LPDIRECT3DCUBETEXTURE9* Get_CubeTex(void) { return &m_pCubeTex;  }
public:
	void Initialize(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;

	unordered_map<const TCHAR*, LPDIRECT3DTEXTURE9> m_mapTexture;
	typedef unordered_map<const TCHAR*, LPDIRECT3DTEXTURE9> MAPTEXTURE;

	LPDIRECT3DCUBETEXTURE9 m_pCubeTex = nullptr;

private:
	virtual void Free(void);
};

END

#endif // !TextureManager
