#include "stdafx.h"
#include "Font_Number.h"
#include "Renderer.h"
#include "InputDev.h"
#include "PlayerObserver.h"
#include "InfoSubject.h"

USING(Engine)

CFont_Number::CFont_Number(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_pPlayerObserver(nullptr)
{
	m_pPlayerObserver = CPlayerObserver::Create();
	CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);
}


CFont_Number::~CFont_Number()
{
}

// 몇 자릿수 인지 구한다
int CFont_Number::Get_Digits(int iNum) 
{
	m_vecDigits = Compute_Digits(iNum);

	return m_vecDigits.size();
}

void CFont_Number::Draw_FontNumber(LPD3DXEFFECT pEffect, float fInterval)
{
	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	for (size_t i = 0; i < m_vecDigits.size(); ++i)
	{
		if (m_vecDigits[i] & 0x80000000) // 음수 X
			continue;

		m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_vecDigits[i]); // 이미지 변경

		m_pTransformCom->Change_WorldMatrix(&_vec3(SCALE_WINCX * 10.f * fInterval, 0.f, 0.f)); // 글자간 간격 10 * fInterval
		m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);
		pEffect->CommitChanges();

		m_pBufferCom->Render_Buffer(); // 그리기
	}
}

void CFont_Number::Free(void)
{
	CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	COrthographicUI::Free();
}
