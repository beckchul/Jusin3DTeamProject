#include "stdafx.h"
#include "UI.h"
#include "Renderer.h"
#include "Mouse_Manager.h"

USING(Engine)

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pTextureCom(nullptr)
{
}

CUI::~CUI()
{
}

vector<int> CUI::Compute_Digits(int iNum)
{
	vector<int> vecDigits;

	do 
	{
		vecDigits.push_back(iNum % 10);
		iNum /= 10;

	} while (iNum > 0);

	reverse(vecDigits.begin(), vecDigits.end());

	return vecDigits;
}

void CUI::Get_Position(void)
{
	// 마우스로 위치 잡기
	_vec3 vecMouse = CMouse_Manager::GetInstance()->GetMousePos();
	cout << "X : " << vecMouse.x << " Y : " << vecMouse.y << endl;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(vecMouse.x - WINCX / 2.f, -vecMouse.y + WINCY / 2.f, 0.f));
}

void CUI::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);

	Engine::CGameObject::Free();
}
