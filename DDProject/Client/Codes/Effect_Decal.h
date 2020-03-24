
#ifndef Effect_Decal_h__
#define Effect_Decal_h__

#include "Effect.h"
#include "Defines.h"

class CEffect_Decal : public Engine::CEffect
{
public:
	explicit	 CEffect_Decal(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Decal(const CEffect_Decal& rhs);
	virtual	~CEffect_Decal(void);

public:
	virtual CEffect_Decal* Clone(void);

public:
//	void Set_FadeOut
	void Set_MaskTextureColor(_vec3 vColor) { m_vMaskColor = vColor; }
	void Set_DecalOp(_float fColorPower, _float fFadeSpeed) 
	{ m_fDecalMask_FadeInTime = fColorPower; m_fFadeSpeed = fFadeSpeed; }

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	virtual HRESULT Add_Component(void);

public:
	_vec4 m_PicColor;

private:
	void Set_TexTag(const _tchar* DiffuseTag, const _tchar* MaskTag);

private:
	const _tchar*							m_pTexTag;
	const _tchar*							m_pMaskTag;

private:
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CTexture*					m_pTextureCom = nullptr;
	Engine::CTexture*					m_pTextureMaskCom = nullptr;
	Engine::CCube_Texture*		m_pBufferCom = nullptr;

private:	// Decal의 FadeIn, Out을 위한 개별적인 변수
	_float		m_fDecalMask_FadeInTime;
	_float		m_fAll_FadeOutTime;
	_float		m_fBlendTime;		// 이 시간이 지나면 마스크맵과 섞어줄거야 .
	_float		m_fColorPower;
	_float		m_fFadeSpeed;
	_bool		m_bUseMaskMap;

private:	
	_bool		m_bUVAnimation;
	_vec2		m_vecUV;
	_vec2		m_vecUVAnimation;
	_float		m_fUVAniSpeed;
	_float		m_fUVTime;		// UV흐르는 애니메이션을 위한 시간. ( 점진적 증가값을 필요로 함 )
	_vec3		m_vMaskColor;

private:
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CEffect_Decal* Create(LPDIRECT3DDEVICE9 pGraphicDev
		, const _tchar* pDiffuseTexTag, const _tchar* pMaskTag = nullptr);

private:
	virtual void Free(void);


};

#endif // Effect_Decal_h__