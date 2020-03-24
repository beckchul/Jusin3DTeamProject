#include "stdafx.h"
#include "Font_Perspective.h"

USING(Engine)

CFont_Perspective::CFont_Perspective(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPerspectiveUI(pGraphicDev)
{
}

CFont_Perspective::~CFont_Perspective()
{
}

void CFont_Perspective::Get_Digits(const _uint uNumber, vector<int>& vecNumber, int & iCount)
{
	while (1)
	{
		int iTemp = (int)pow(10, iCount);
		iCount++;

		vecNumber.push_back(uNumber / iTemp % 10);

		if (uNumber / (iTemp * 10) < 1)
			break;
	}
}

void CFont_Perspective::Render_Number(const _uint uNumber, LPD3DXEFFECT pEffect, _float fInterval)
{
	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	// 자릿수에 따라서
	vector<int> vecNumber;
	vecNumber.reserve(3);
	int iCount = 0;
	Get_Digits(uNumber, vecNumber, iCount);

	D3DXVECTOR3 vecRight; // 오른쪽 벡터를 구해서 이동시키자
	memcpy(&vecRight, (*m_pTransformCom->Get_WorldMatrix()).m[0], sizeof(D3DXVECTOR3));
	D3DXVec3Normalize(&vecRight, &vecRight);

	m_pTransformCom->Change_WorldMatrix(&(-vecRight * fInterval * (iCount / 2.f))); // 글자를 가운데로 놓기 위함
																			   // Render
	for (int i = 0, j = 0; i < iCount; ++i, ++j)
	{
		m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

		if (iCount >= 4 && (j == (iCount - 3))) // ,
		{
			m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 10);
			i--;
		}
		else
			m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", vecNumber[iCount - i - 1]);

		pEffect->CommitChanges();

		m_pBufferCom->Render_Buffer();

		m_pTransformCom->Change_WorldMatrix(&(vecRight * fInterval)); // 글자간 간격
	}
}

void CFont_Perspective::Free(void)
{
	CPerspectiveUI::Free();
}
