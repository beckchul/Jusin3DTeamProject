#pragma once

#include "PerspectiveUI.h"

namespace Engine
{
	class CDataManager;
}
class CUI_Skill : public CPerspectiveUI
{
public:
	explicit CUI_Skill(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTexture);
	virtual ~CUI_Skill();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CUI_Skill* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTexture);
private:
	_uint				m_iTextureIdx = 0;
	CDataManager*		m_pDataMgr = nullptr;
private:
	FRAME				m_tFrame;
private:
	virtual void Free(void);
};

