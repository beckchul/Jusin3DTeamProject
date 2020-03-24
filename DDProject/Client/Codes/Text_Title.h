#pragma once

#include "OrthographicUI.h"

class CText_Title : public COrthographicUI
{
public:
	explicit CText_Title(LPDIRECT3DDEVICE9 pGraphicDev, _int iTexture, _float fStart, _float fFinish);
	virtual ~CText_Title();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	_float		m_fTime = 0.f;
	_float		m_fAlpha = 0.f;
	_int		m_iTexture = 0;

	_float		m_fStartTime = 0.f;
	_float		m_fFinishTime = 0.f;
public:
	static CText_Title* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iTexture, _float fStart, _float fFinish);
private:
	virtual void Free(void);
};

