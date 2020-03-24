#ifndef Texture_h__
#define Texture_h__

#include "Resources.h"

BEGIN(Engine)

class ENGINE_DLL CTexture : public CResources
{
public:
	enum TEXTURETYPE {TYPE_NORMAL, TYPE_CUBE, TYPE_END};
protected:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture(void);
public:
	void SetUp_OnGraphicDev(const _uint& uIndex = 0);
	void SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName, const _uint& uIndex = 0);
public:
	virtual HRESULT Ready_Texture(const _tchar* pPath, TEXTURETYPE eType, const _uint& iCnt);

private:
	vector<IDirect3DBaseTexture9*>				m_vecTexture;
	typedef vector<IDirect3DBaseTexture9*>		VECTEXTURE;

public:
	virtual CComponent* Clone(void);
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath, TEXTURETYPE eType, const _uint& iCnt = 1);
public:
	virtual void Free(void);
	
};


END

#endif // Texture_h__
