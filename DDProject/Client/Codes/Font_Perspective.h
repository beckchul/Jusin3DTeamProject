#pragma once

#include "PerspectiveUI.h"

class CFont_Perspective : public CPerspectiveUI
{
public:
	explicit CFont_Perspective(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFont_Perspective();
protected:
	void Get_Digits(const _uint uNumber, vector<int>& vecNumber, int& iCount);
	void Render_Number(const _uint uNumber, LPD3DXEFFECT pEffect, _float fInterval = 1.5f);

protected:
	virtual void Free(void);

};