#pragma once

#include "OrthographicUI.h"

class CFont_Player : public COrthographicUI
{
public:
	explicit CFont_Player(LPDIRECT3DDEVICE9 pGraphicDev, _int iClientIdx);
	virtual ~CFont_Player();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
	void Set_Pos(_vec3& vecPos);
private:
	_int			m_iClientIdx = 0;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CFont_Player* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iClientIdx);
private:
	virtual void Free(void);
};

