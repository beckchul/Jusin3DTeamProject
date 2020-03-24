#pragma once

#include "Defines.h"
#include "StaticObject.h"

class CStage2_SkyBox : public CStaticObject
{
public:
	explicit CStage2_SkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage2_SkyBox();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual int FirstUpdate_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	float			m_fRadius = 0.f;
	float			m_fTime = 0.f;
public:
	static CStage2_SkyBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

