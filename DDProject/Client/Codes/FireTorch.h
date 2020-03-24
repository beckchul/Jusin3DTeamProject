#pragma once

#include "Defines.h"
#include "GameObject.h"

class CFireTorch : public Engine::CGameObject
{
public:
	explicit CFireTorch(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFireTorch();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	Engine::CStaticMesh*		m_pMeshCom = nullptr;

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CFireTorch* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

