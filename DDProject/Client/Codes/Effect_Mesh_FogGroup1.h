
#ifndef Effect_Mesh_FogGroup1_h__
#define Effect_Mesh_FogGroup1_h__

#include "Effect_Mesh.h"

class CEffect_Mesh_FogGroup1 : public CEffect_Mesh
{
public:
	explicit	 CEffect_Mesh_FogGroup1(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Mesh_FogGroup1(const CEffect_Mesh_FogGroup1& rhs);
	virtual	~CEffect_Mesh_FogGroup1(void);

public:
	virtual CEffect_Mesh_FogGroup1* Clone(void);

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);

private:
	Engine::CTexture*				m_pTextureMaskCom = nullptr;

public:
	static CEffect_Mesh_FogGroup1* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);
};

#endif Effect_Mesh_FogGroup1_h__