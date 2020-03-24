
#ifndef Effect_Mesh_Wave_h__
#define Effect_Mesh_Wave_h__

#include "Effect_Mesh.h"

class CEffect_Mesh_Wave : public CEffect_Mesh
{
public:
	explicit	 CEffect_Mesh_Wave(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Mesh_Wave(const CEffect_Mesh_Wave& rhs);
	virtual	~CEffect_Mesh_Wave(void);

public:
	virtual CEffect_Mesh_Wave* Clone(void);

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CEffect_Mesh_Wave* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);
};

#endif Effect_Mesh_Wave_h__