#include "stdafx.h"
#include "StaticUI.h"
#include "Renderer.h"
#include "WaveObserver.h"
#include "InfoSubject.h"
#include "Mouse_Manager.h"

#include "Text_Name_Level.h"
#include "Text_Score.h"
#include "Text_Wave.h"
#include "Text_Build.h"
#include "Text_Combat.h"
#include "Text_DefenseUnits.h"
#include "Text_NextEnemies.h"
#include "Text_Boss.h"
#include "Text_RespawnIn.h"

#include "Panel_Level.h"
#include "Panel_SkillIcon.h"
#include "Panel_Wave.h"
#include "Panel_newBar.h"
#include "Panel_MonsterKill.h"
#include "Panel_ExpBar.h"
#include "Panel_Boss.h"

#include "Font_Score.h"
#include "Font_Level.h"
#include "Font_Hp.h"
#include "Font_Mp.h"
#include "Font_MaxHp.h"
#include "Font_MaxMp.h"
#include "Font_Wave.h"
#include "Font_DefenseUnits.h"
#include "Font_NextEnemies.h"
#include "Font_FPS.h"
#include "Font_KillEnemies.h"
#include "Font_Exp.h"
#include "Font_BossHP.h"
#include "Font_RespawnTime.h"

#include "MousePointer.h"
#include "MiniMap.h"

#include "Vertical_Hp.h"
#include "Vertical_Mp.h"

USING(Engine)

CStaticUI::CStaticUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_pWaveObserver(nullptr)
{
}


CStaticUI::~CStaticUI()
{
}

HRESULT CStaticUI::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	COrthographicUI* pUI = nullptr;

	pUI = CVertical_Mp::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CVertical_Hp::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);

	pUI = CPanel_Level::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CPanel_newBar::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CPanel_ExpBar::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CPanel_SkillIcon::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CPanel_Wave::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CPanel_MonsterKill::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CPanel_Boss::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);

	pUI = CText_Name_Level::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CText_Score::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CText_Wave::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CText_Build::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CText_Combat::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CText_DefenseUnits::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CText_NextEnemies::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CText_Boss::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CText_RespawnIn::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);

	pUI = CFont_Score::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_Level::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_Hp::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_Mp::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_MaxHp::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_MaxMp::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_Wave::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_DefenseUnits::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_NextEnemies::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_FPS::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_KillEnemies::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_Exp::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_BossHP::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	pUI = CFont_RespawnTime::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	
	pUI = CMousePointer::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);
	Engine::CMouse_Manager::GetInstance()->Set_MousePointer(pUI);

	pUI = CMiniMap::Create(m_pGraphicDev); m_listTextUI.push_back(pUI);

	m_pWaveObserver = CWaveObserver::Create(m_pGraphicDev);
	CInfoSubject::GetInstance()->Subscribe_Observer(m_pWaveObserver);

	return NOERROR;
}

int CStaticUI::Update_GameObject(const float & fTimeDelta)
{
	if (TRUE == m_pWaveObserver->Get_WaveInfo().bCutScene)
		return 0;

	list<COrthographicUI*>::iterator iter_begin = m_listTextUI.begin();
	list<COrthographicUI*>::iterator iter_end = m_listTextUI.end();

	for (; iter_begin != iter_end; ++iter_begin)
		(*iter_begin)->Update_GameObject(fTimeDelta);

	return 0;
}

CStaticUI * CStaticUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStaticUI* pInstance = new CStaticUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CText Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CStaticUI::Free(void)
{
	list<COrthographicUI*>::iterator iter_begin = m_listTextUI.begin();
	list<COrthographicUI*>::iterator iter_end = m_listTextUI.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		Engine::Safe_Release(*iter_begin);
	}

	CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pWaveObserver);
	Engine::Safe_Release(m_pWaveObserver);

	COrthographicUI::Free();
}
