#pragma once

#include "PerspectiveUI.h"

namespace Engine
{
	class CDataManager;
}

class CPlayerObserver;
class CUI_MapTitle : public CPerspectiveUI
{
public:
	explicit CUI_MapTitle(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTexture);
	virtual ~CUI_MapTitle();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CUI_MapTitle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTexture);
private:
	_uint				m_iTextureIdx = 0;
	CDataManager*		m_pDataMgr = nullptr;
	CPlayerObserver*	m_pPlayerObserver = nullptr;
private:
	virtual void Free(void);
};

