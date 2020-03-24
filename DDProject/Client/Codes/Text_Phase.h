#pragma once

#include "OrthographicUI.h"

class CText_Phase : public COrthographicUI
{
public:
	explicit CText_Phase(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTextureNum);
	virtual ~CText_Phase();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	_uint m_iTextureNum = 0;
	bool  m_bReadyToDie = false;
	float m_fTime = 0.f; // »î°ú Á×À½ÀÇ ½Ã°£
	float m_fAlpha = 1.f;
public:
	static CText_Phase* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTextureNum);
private:
	virtual void Free(void);
};

