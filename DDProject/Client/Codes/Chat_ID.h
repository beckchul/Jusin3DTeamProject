#pragma once

#include "OrthographicUI.h"

class CChat_ID : public COrthographicUI
{
public:
	explicit CChat_ID(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex);
	virtual ~CChat_ID();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	string			m_szMessage;
	_float			m_fTime = 0.f;;  // ���� �ð�
	_int			m_iClientIdx = 0; // ������ ä������
	map<CHAR, _int>	m_mapChat; // ä�� �޼��� ����
public:
	static CChat_ID* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex);
private:
	virtual void Free(void);
};

