#pragma once

#include "Font_Perspective.h"

namespace Engine
{
	class CDataManager;
}
class CFont_Damage : public CFont_Perspective
{
public:
	explicit CFont_Damage(LPDIRECT3DDEVICE9 pGraphicDev, _uint uDamage);
	virtual ~CFont_Damage();
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
	static CFont_Damage* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint uDamage);
private:
	virtual void Free(void);
};

