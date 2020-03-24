
#ifndef Effect_Trail_h__
#define Effect_Trail_h__

#include "Effect.h"
#include "Defines.h"

class CEffect_Trail : public Engine::CEffect
{
public:
	explicit	 CEffect_Trail(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Trail(const CEffect_Trail& rhs);
	virtual	~CEffect_Trail(void);

public:
	virtual CEffect_Trail* Clone(void);

public:		// Setter
	void Set_MatrixInfo(const D3DXMATRIX* pPlayerMat, const D3DXMATRIX* pPlayerWeaponMat)
	{
		m_pmatPlayerInfo = pPlayerMat;
		m_pmatWeaponRef = pPlayerWeaponMat;
	}

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	virtual HRESULT Add_Component(void);

private:		
	void Memorize_Point(const float & fTimeDelta);

private:
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;
	Engine::CTrail_Texture*			m_pBufferCom = nullptr;

private:
	const _matrix*		m_pmatPlayerInfo;
	const _matrix*		m_pmatWeaponRef;

private:
	list<_vec3>				m_Pointlist;
	typedef list<_vec3>	POINTLIST;

private:		// 트레일 속성
	_int			m_iTrailCount;
	_float			m_fTrailEmitRate;
	_float			m_fTrailSize;

private:
	_float			m_fAccTime;
	_float			m_fTimeDelta;
private:
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CEffect_Trail* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);

};

#endif // Effect_Trail_h__