#include "stdafx.h"
#include "OrthographicUI.h"
#include "Renderer.h"

USING(Engine)

COrthographicUI::COrthographicUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
	, m_fX(0.f)
	, m_fY(0.f)
	, m_fSizeX(0.f)
	, m_fSizeY(0.f)
{
}

COrthographicUI::~COrthographicUI()
{
}

void COrthographicUI::Free(void)
{

	CUI::Free();
}
