#pragma once

#include "OrthographicUI.h"

class CPlayerObserver;
class CFont_Number : public COrthographicUI
{
public:
	explicit CFont_Number(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont_Number();
protected:
	CPlayerObserver*		m_pPlayerObserver = nullptr;
	vector<int>				m_vecDigits; // ÀÚ¸´ ¼ö
protected:
	int Get_Digits(int iNum);
	void Draw_FontNumber(LPD3DXEFFECT pEffect, float fInterval = 1.f);
protected:
	virtual void Free(void);
};

