
#ifndef Effect_Mesh_LightningTree_h__
#define Effect_Mesh_LightningTree_h__

#include "Effect_Mesh.h"

class CEffect_Mesh_LightningTree : public CEffect_Mesh
{
public:
	explicit	 CEffect_Mesh_LightningTree(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Mesh_LightningTree(const CEffect_Mesh_LightningTree& rhs);
	virtual	~CEffect_Mesh_LightningTree(void);

public:
	virtual CEffect_Mesh_LightningTree* Clone(void);

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CEffect_Mesh_LightningTree* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);
};

#endif Effect_Mesh_LightningTree_h__