#pragma once

#include "Defines.h"
#include "StaticObject.h"


// 이 녀석들만 따로 카툰으로 그려보자~

class CFrustumManager;
class CRibbon : public CStaticObject
{
public:
	explicit CRibbon(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex);
	virtual ~CRibbon();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual int FirstUpdate_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
	void Calculate_Raidus(void);
	void Calculate_LocalPos(void);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	CFrustumManager*	m_pFrustumMgr = nullptr;
private:
	_int			m_iTextureNum = 0;
	float			m_fRadius = 0.f;
	float			m_fTime = 0.f;
public:
	static CRibbon* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex);
private:
	virtual void Free(void);
};

