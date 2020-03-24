#include "stdafx.h"
#include "PerspectiveUI.h"

USING(Engine)

CPerspectiveUI::CPerspectiveUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
	, m_pShaderCom(nullptr)
{
}

CPerspectiveUI::~CPerspectiveUI()
{
}

void CPerspectiveUI::Free(void)
{
	Safe_Release(m_pShaderCom);
	CUI::Free();
}
