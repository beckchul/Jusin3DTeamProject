
#ifndef Effect_Single_Fog_h__
#define Effect_Single_Fog_h__

#include "Defines.h"
#include "Effect_2D.h"

class CEffect_Single_Fog : public CEffect_2D
{
protected:
	explicit	 CEffect_Single_Fog(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Single_Fog(const CEffect_Single_Fog& rhs);
	virtual	~CEffect_Single_Fog(void);

public:
	virtual CEffect_Single_Fog* Clone(void);

public:
	void Set_FogFadeSpeed(_float fSpeed) { m_fFadeSpeed = fSpeed; }

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	virtual HRESULT Add_Component(void);

private:
	void Set_Tag(SCENEID eID);

private:
	SCENEID	m_eSceneID;
	_float			m_fFadeSpeed = -0.03f;


private:
	Engine::CTexture*				m_pTextureMaskCom = nullptr;
	Engine::CTexture*				m_pTextureFaderCom = nullptr;

private:
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CEffect_Single_Fog* Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENEID eID);
private:
	virtual void Free(void);
};

#endif // Effect_Single_Fog_h__