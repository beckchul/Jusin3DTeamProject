#pragma once

#include "PerspectiveUI.h"

namespace Engine
{
	class CDataManager;
}
class CMapTexture : public CPerspectiveUI
{
public:
	explicit CMapTexture(LPDIRECT3DDEVICE9 pGraphicDev, Engine::OBJECTID eObjectId);
	virtual ~CMapTexture();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CMapTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::OBJECTID eObjectId);
private:
	_uint				m_iTextureIdx = 0;
	CDataManager*		m_pDataMgr = nullptr;
private:
	virtual void Free(void);
};

