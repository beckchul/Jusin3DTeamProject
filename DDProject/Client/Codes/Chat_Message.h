#pragma once

#include "OrthographicUI.h"

class CFont_Player;
class CChat_ID;
class CChat_Message : public COrthographicUI
{
public:
	explicit CChat_Message(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex, bool bChatTemp = false);
	virtual ~CChat_Message();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
	void Set_Message(const char szMessage[128]) { strcpy_s(m_szMessage, sizeof(m_szMessage), szMessage); }
	void Set_Order(_int iOrder) { m_iOrder = iOrder; }
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	CHAR			m_szMessage[128] = "";
	_float			m_fTime = 0.f;;  // ���� �ð�
	_int			m_iClientIdx = 0; // ������ ä������
	_int			m_iOrder = 0; // ����
	_bool			m_bChatTemp = false; // �ӽ� ä��â
	map<CHAR, _int>	m_mapChat; // ä�� �޼��� ����

	Engine::CTexture*	 m_pTextureChat[2];

	CFont_Player*		 m_pFont_Player = nullptr;
	CChat_ID*			 m_pChat_ID = nullptr;
public:
	static CChat_Message* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex, bool bChatTemp = false);
private:
	virtual void Free(void);
};

