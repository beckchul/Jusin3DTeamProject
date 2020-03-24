#pragma once

#include "OrthographicUI.h"

class CSkillIcon;
class CPanel_SkillIcon : public COrthographicUI
{
public:
	explicit CPanel_SkillIcon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPanel_SkillIcon();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
	HRESULT CreateSkillIcon(void);
private:
	CSkillIcon*		m_arrSkillIcon[SKILL_END];
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CPanel_SkillIcon* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

