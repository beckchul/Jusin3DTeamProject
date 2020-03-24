
#ifndef MyEffect_h__
#define MyEffect_h__

#include "Effect.h"
#include "Defines.h"

class CMyEffect : public Engine::CEffect
{
protected:
	explicit CMyEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMyEffect(void);

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	virtual HRESULT Add_Component(void);
public:
	virtual CEffect* Clone(void);

private:
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;
	Engine::CRect_Texture*			m_pBufferCom = nullptr;

private:
	FRAME									m_tFrame;

private:
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CMyEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

#endif // MyEffect_h__