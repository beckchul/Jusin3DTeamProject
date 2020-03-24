#pragma once

#include "OrthographicUI.h"

class CPlayerObserver;
class CSkillIcon : public COrthographicUI
{
public:
	explicit CSkillIcon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSkillIcon();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
	void SetInfo(SKILL_TYPE eType, _vec2 vecPos);
private:
	CPlayerObserver*			m_pPlayerObserver = nullptr;
	PLAYER_TYPE					m_ePlayerType = PLAYER_TYPE_END;
private:
	SKILL_TYPE				m_eSkillType = SKILL_END;
	int						m_iMPrequired = 0; // 필요 마나
	bool					m_bActivated = false;
private:
	Engine::CTexture* m_pTexture_Skill[PLAYER_TYPE_END];
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CSkillIcon* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

