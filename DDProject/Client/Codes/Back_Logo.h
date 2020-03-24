#pragma once

#include "Defines.h"
#include "GameObject.h"

class CBack_Logo : public Engine::CGameObject
{
public:
	explicit CBack_Logo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBack_Logo();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CRect_Texture*		m_pBufferCom = nullptr;
	
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	int		m_iRand = 0;
public:
	static CBack_Logo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};
