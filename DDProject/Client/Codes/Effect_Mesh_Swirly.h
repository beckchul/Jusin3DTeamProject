
#ifndef Effect_Mesh_Swirly_h__
#define Effect_Mesh_Swirly_h__

#include "Effect_Mesh.h"

class CEffect_Mesh_Swirly : public CEffect_Mesh
{
public:
	explicit	 CEffect_Mesh_Swirly(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Mesh_Swirly(const CEffect_Mesh_Swirly& rhs);
	virtual	~CEffect_Mesh_Swirly(void);

public:
	virtual CEffect_Mesh_Swirly* Clone(void);

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CEffect_Mesh_Swirly* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);
};

#endif Effect_Mesh_Swirly_h__