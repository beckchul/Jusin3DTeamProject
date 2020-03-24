#pragma once

#include "OrthographicUI.h"

namespace Engine
{
	class CInputDev;
}

class CUI_Check;
class CChat_Message;
class CPanel_Login : public COrthographicUI
{
public:
	explicit CPanel_Login(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPanel_Login();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	void KeyCheck(void);
private:
	CInputDev*		m_pInputDev = nullptr;
	char			m_szTemp[1024];
	_float			m_fEnterTime = 0.f;
	_bool			m_bSubmit = false;
private:
	BYTE			m_Alphabet_KeyArray[26]; // a ~ z
	BYTE			m_Number_KeyArray[10]; // 0 ~ 9

	CChat_Message*	m_pTempChatMessage = nullptr;
	CChat_Message*	m_pCurrentId = nullptr;
	CUI_Check*		m_pUICheck = nullptr;
public:
	static CPanel_Login* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

