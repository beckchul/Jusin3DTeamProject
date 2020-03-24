#pragma once

#include "UI.h"

class CPerspectiveUI : public CUI
{
public:
	explicit CPerspectiveUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPerspectiveUI();
protected:
	Engine::CShader*			m_pShaderCom = nullptr;
protected:
	virtual void Free(void);

};