#pragma once

#include "UI.h"

class COrthographicUI : public CUI
{
public:
	explicit COrthographicUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~COrthographicUI();

protected:
	float			m_fX = 0.f, m_fY = 0.f;
	float			m_fSizeX = 0.f, m_fSizeY = 0.f;
protected:
	virtual void Free(void);
};

