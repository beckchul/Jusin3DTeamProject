#pragma once

#include "OrthographicUI.h"
#include "Mouse_Manager.h"

namespace Engine
{
	class CMouse_Manager;
	class CTexture;
}

class CMousePointer : public COrthographicUI
{
public:
	explicit CMousePointer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMousePointer();
	// setter
public:
	void Set_ShowMousePointer(bool b) { m_bShowMouse = b; }
	void Set_TowerPosition(_vec3& vecPos, POINT& ptDir);
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
	void Change_MousePointer(CMouse_Manager::eMousePointer ePointer);
	void Change_Color(_vec4& vecColor) { m_vecColor = vecColor; }
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
	void Rotate_By_Tower(void);
public:
	static CMousePointer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	CMouse_Manager*						m_pMouseMgr = nullptr;
	CMouse_Manager::eMousePointer		m_eMousePointer = CMouse_Manager::POINTER_END;
private:
	_vec4			m_vecColor;
	bool			m_bShowMouse = true;
	_vec3			m_TowerPosition;
private:
	Engine::CTexture*		m_pTextureCom2 = nullptr;
private:
	virtual void Free(void);
};

