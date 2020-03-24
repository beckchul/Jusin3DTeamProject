#pragma once

#include "PerspectiveUI.h"

namespace Engine
{
	class CDataManager;
}

class CPlayer_ID : public CPerspectiveUI
{
public:
	explicit CPlayer_ID(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex);
	virtual ~CPlayer_ID();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	string			m_szMessage;
	CDataManager*	m_pDataMgr = nullptr;
	_int			m_iClientIdx = 0; // 누구의 채팅인지
	map<CHAR, _int>	m_mapChat; // 채팅 메세지 보관
public:
	static CPlayer_ID* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex);
private:
	virtual void Free(void);
};

