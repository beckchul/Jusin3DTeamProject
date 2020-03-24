#pragma once

#include "Font_Perspective.h"

namespace Engine
{
	class CDataManager;
}
class CFont_PlayerDamage : public CFont_Perspective
{
public:
	explicit CFont_PlayerDamage(LPDIRECT3DDEVICE9 pGraphicDev, _uint uDamage);
	virtual ~CFont_PlayerDamage();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	_uint				m_uDamage = 0;
	_float				m_fTime = 0.f;
	_vec3				m_vecScale;
	CDataManager*		m_pDataMgr = nullptr;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CFont_PlayerDamage* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint uDamage);
private:
	virtual void Free(void);
};

