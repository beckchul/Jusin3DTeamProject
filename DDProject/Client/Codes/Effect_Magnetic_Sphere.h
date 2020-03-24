
#ifndef Effect_Magnetic_Sphere_h__
#define Effect_Magnetic_Sphere_h__

#include "Effect_Mesh.h"

namespace Engine
{
	class CCollisionSphere;
}

class CCollisionManager;

class CEffect_Magnetic_Sphere : public CEffect_Mesh
{
public:
	enum MAGNETIC_STATE { ACTIVE, PREVIEW, STATE_END };

public:
	explicit	 CEffect_Magnetic_Sphere(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Magnetic_Sphere(const CEffect_Magnetic_Sphere& rhs);
	virtual	~CEffect_Magnetic_Sphere(void);

public:
	virtual CEffect_Magnetic_Sphere* Clone(void);

public:
	void Set_MagneticState(MAGNETIC_STATE eState) { m_eState = eState; }
	void Set_InitMagnetic(_int iRound);
	void Set_RenderState(_bool flag) { m_bRender = flag; }
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);

private:
	Engine::CTexture*					m_pTextureMaskCom = nullptr;
	Engine::CTexture*					m_pTextureNormalCom = nullptr;
	_float									m_fScale = 15.f;
	_float									m_fCollisionScale = 1500.f;
	Engine::CCollisionSphere*		m_pCollisionSphere = nullptr;
	CCollisionManager*				m_pCollisionManager = nullptr;
	MAGNETIC_STATE				m_eState;
	_bool									m_bRender;
	_vec4									m_vPreviewColor;
	_vec4									m_vRealColor;
	_uint									m_iPos = 0;
public:
	static CEffect_Magnetic_Sphere* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);
};

#endif Effect_Magnetic_Sphere_h__