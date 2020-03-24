
#ifndef Effect_Mesh_Shield_h__
#define Effect_Mesh_Shield_h__

#include "Effect_Mesh.h"

class CEffect_Mesh_Shield : public CEffect_Mesh
{
public:
	explicit	 CEffect_Mesh_Shield(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Mesh_Shield(const CEffect_Mesh_Shield& rhs);
	virtual	~CEffect_Mesh_Shield(void);

public:
	virtual CEffect_Mesh_Shield* Clone(void);

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);

private:
	Engine::CTexture*				m_pTextureMaskCom = nullptr;
	Engine::CTexture*				m_pTextureNormalCom = nullptr;
	_bool								m_bIsCreateUI = FALSE;
	AURA_TYPE						m_eAuraType = AURA_END;

public:
	static CEffect_Mesh_Shield* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);
};

#endif Effect_Mesh_Shield_h__