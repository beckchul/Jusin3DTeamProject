#include "stdafx.h"
#include "WaveObserver.h"
#include "Camera_Manager.h"

#include "InfoSubject.h"
#include "Object_Manager.h"
#include "Text_Phase.h"
#include "SoundMgr.h"

CWaveObserver::CWaveObserver(LPDIRECT3DDEVICE9 pGraphicDev)
	:m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();

	ZeroMemory(&m_tWaveInfo, sizeof(WAVE_INFO));
	m_tWaveInfo.iCurrentWave = 1;
	m_tWaveInfo.iTotalWave = 6;
	m_tWaveInfo.iTotalUnits = 8;
}

CWaveObserver::~CWaveObserver(void)
{
}

void CWaveObserver::Update_Observer(int message)
{
	const list<void*>* pDatalist = Engine::CInfoSubject::GetInstance()->GetDatalist(message);

	if (nullptr == pDatalist)
		return;

	switch (message)
	{
	case MESSAGE_PHASECHANGE:
		m_tWaveInfo.ePhaseType = *(PHASE_TYPE*)pDatalist->back();
		Text_Phase(m_tWaveInfo.ePhaseType); // 텍스트 띄우기
		break;
	case MESSAGE_BUILD_PHASECHANGE:
		m_tWaveInfo.ePhaseType = BUILD_PHASE;
		Text_Phase(m_tWaveInfo.ePhaseType); // 텍스트 띄우기
		break;
	case MESSACE_CUTSCENE:
		m_tWaveInfo.bCutScene = *(bool*)pDatalist->back();
		break;
	case MESSAGE_PLUSCOUNT:
		++m_tWaveInfo.iCurUnits;
		break;
	case MESSAGE_MINUSCOUNT:
		--m_tWaveInfo.iCurUnits;
		if (0 == m_tWaveInfo.iCurUnits)
		{
			m_tWaveInfo.ePhaseType = WAVE_COMPLETE;
			++m_tWaveInfo.iCurrentWave;

			Text_Phase(m_tWaveInfo.ePhaseType); // 텍스트 띄우기
			CSoundMgr::GetInstance()->Set_BGMVolume(Engine::CHANNEL_STAGE, 0.1f);
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_WAVE))
				CSoundMgr::GetInstance()->Play_Sound(L"DD_WaveFinalbeat.ogg", Engine::CHANNEL_WAVE, 0.8f);
		}
		break;
	case MESSAGE_MAX_UNITCOUNT:
		m_tWaveInfo.iTotalUnits = *(int*)pDatalist->back();
		break;
	case MESSAGE_BOSS_APPEAR:
		m_tWaveInfo.bBossExist = !*(bool*)pDatalist->back(); // m_bDead로 받으니까 !로 바꿔주자
		
		if (m_tWaveInfo.bBossExist)
			CCamera_Manager::GetInstance()->Change_Camera(CAMERA_ACTION, CUTSCENE_BOSS);
		else
			g_eCurrentScene = SCENE_STAGE2;
		break;
	case MESSAGE_BOSS_HP:
		m_tWaveInfo.iBossHp = *(int*)pDatalist->back();
		break;
	case MESSAGE_STAGE_CLEAR:
		m_tWaveInfo.bWaveClear = true;
	default:
		break;
	}

}

HRESULT CWaveObserver::Ready_Observer(void)
{
	return NOERROR;
}

void CWaveObserver::Text_Phase(PHASE_TYPE eType)
{
	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CText_Phase::Create(m_pGraphicDev, eType);
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject);
}

CWaveObserver* CWaveObserver::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWaveObserver*		pInstance = new CWaveObserver(pGraphicDev);

	if (FAILED(pInstance->Ready_Observer()))
	{
		MSG_BOX(L"CWaveObserver Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void CWaveObserver::Free(void)
{
	Engine::Safe_Release(m_pGraphicDev);
}

