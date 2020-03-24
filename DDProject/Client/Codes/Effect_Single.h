
#ifndef Effect_Single_h__
#define Effect_Single_h__

#include "Defines.h"
#include "Effect_2D.h"

class CEffect_Single : public CEffect_2D
{
protected:
	explicit	 CEffect_Single(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Single(const CEffect_Single& rhs);
	virtual	~CEffect_Single(void);

public:
	virtual CEffect_Single* Clone(void);

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	virtual HRESULT Add_Component(void);

private:
	void Set_TexTag(const _tchar* Tag, SCENEID eID);

private:
	const _tchar*							m_pTexTag;
	SCENEID							m_eSceneID;

private:
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CEffect_Single* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTexTag, SCENEID eID);
private:
	virtual void Free(void);
};

#endif // Effect_Single_h__