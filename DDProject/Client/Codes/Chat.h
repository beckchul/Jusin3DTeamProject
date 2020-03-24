#pragma once

#include "OrthographicUI.h"

namespace Engine
{
	class CInputDev;
}

class CChat_Message;
class CChat : public COrthographicUI
{
public:
	explicit CChat(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CChat();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
	void Get_ServerChat(Engine::NETDATA tData);
private:
	void KeyCheck(void);
private:
	CInputDev*		m_pInputDev = nullptr;
private:
	bool			m_bCharMode = false;
	char			m_szTemp[1024];
private:
	BYTE			m_Alphabet_KeyArray[26]; // a ~ z
	BYTE			m_Number_KeyArray[10]; // 0 ~ 9
private:
	CChat_Message*		m_pTempChatMessage = nullptr;
	list<CChat_Message*> m_listChatMessage;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CChat* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

