#pragma once

#include "Defines.h"
#include "GameObject.h"

class CTerrain : public Engine::CGameObject
{
public:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTerrain();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTerrain_Texture*	m_pBufferCom = nullptr;
private:
	LPDIRECT3DTEXTURE9			m_pFilterTexture = nullptr;

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT Ready_FilterTexture(void);
public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

