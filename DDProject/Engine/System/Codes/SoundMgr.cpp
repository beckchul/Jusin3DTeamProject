#include "SoundMgr.h"

IMPLEMENT_SINGLETON(Engine::CSoundMgr)

Engine::CSoundMgr::CSoundMgr(void)
	: m_pSystem(nullptr)
{
	Initialize();
}

Engine::CSoundMgr::~CSoundMgr(void)
{
}

void Engine::CSoundMgr::Initialize(void)
{
	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, CHANNEL_END, FMOD_INIT_NORMAL, NULL);
}

void Engine::CSoundMgr::LoadSoundFile()
{
	_finddata_t	fd = {};
	long handle;
	int iResult = 0;

	handle = _findfirst("../Resources/Sound/*.*", &fd);

	if (-1 == handle)
	{
		//MessageBox(hwnd, L"Not found soundfile", L"Not found soundfile", MB_OK);
		return;
	}

	while (-1 != iResult)
	{
		TCHAR*	pSoundKey = new TCHAR[256];
		ZeroMemory(pSoundKey, sizeof(TCHAR) * 256);

		MultiByteToWideChar(CP_ACP, 0, fd.name, strlen(fd.name) + 1, pSoundKey, 256);

		char szFullPath[256] = "";
		strcpy_s(szFullPath, 256, "../Resources/Sound/");
		strcat_s(szFullPath, 256, fd.name);

		FMOD_SOUND*	pSound;

		FMOD_RESULT FResult = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, NULL, &pSound);

		if (FMOD_OK == FResult)
			m_MapSound.insert(make_pair(pSoundKey, pSound));
		else
		{
			//MessageBox(hwnd, L"Sound load failed", L"Sound load failed", MB_OK);
			delete[] pSoundKey;
		}

		iResult = _findnext(handle, &fd);
	}

	_findclose(handle);

	FMOD_System_Update(m_pSystem);
}

void Engine::CSoundMgr::Play_Sound(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, _float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_MapSound.begin(),
		m_MapSound.end(), CTag_Finder(pSoundKey));

	if (iter == m_MapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, 0, &(m_pChannel[eChannel]));
	FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume);
}

void Engine::CSoundMgr::UpdateSound(void)
{
	FMOD_System_Update(m_pSystem);
}

void Engine::CSoundMgr::StopSound(CHANNEL_TYPE eChannel)
{
	FMOD_Channel_Stop(m_pChannel[eChannel]);
}

void Engine::CSoundMgr::StopSoundAll(void)
{
	for (int i = 0; i < CHANNEL_END; ++i)
		FMOD_Channel_Stop(m_pChannel[i]);
}

void Engine::CSoundMgr::Play_Sound(TCHAR * pSoundKey, CHANNEL_TYPE eChannel, _vec3 vecCamEye, _vec3 vecSoundPos, _float fRange, _int iSoundNum, _bool bPlayOnce)
{
	if (bPlayOnce) // 소리가 한 번만 나고싶을 때
	{
		if (CSoundMgr::GetInstance()->IsPlaying(eChannel))
			return;
	}

	float fDistance = D3DXVec3Length(&(vecCamEye - vecSoundPos));
	if (fDistance > fRange)
		return;
	
	// 거리에 따른 소리의 감소도 추가하고 싶으면 할 것.
	
	if (iSoundNum != 1) // 랜덤 사운드
		Play_RandomSound(pSoundKey, eChannel, iSoundNum);
	else
		Play_Sound(pSoundKey, eChannel);
}

void Engine::CSoundMgr::Play_RandomSound(TCHAR * pSoundKey, CHANNEL_TYPE eChannel, int iNum, _float fVolume)
{
	TCHAR szTemp[128] = {};
	lstrcpy(szTemp, pSoundKey);

	lstrcpyn(szTemp, szTemp, lstrlen(szTemp) - 3); // 0.ogg 버리기

	wsprintf(szTemp, L"%s%d%s", szTemp, rand() % iNum + 1, L".ogg");

	CSoundMgr::GetInstance()->Play_Sound(szTemp, eChannel);
	FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume);
}

bool Engine::CSoundMgr::IsPlaying(CHANNEL_TYPE eChannel)
{
	FMOD_BOOL b;
	FMOD_Channel_IsPlaying(m_pChannel[eChannel], &b);

	if (b == 0)
		return false;
	else
		return true;
}

void Engine::CSoundMgr::PlayBGM(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, _float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_MapSound.begin(),
		m_MapSound.end(), CTag_Finder(pSoundKey));

	if (iter == m_MapSound.end())
		return;

	FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);
	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, 0, &(m_pChannel[eChannel]));
	FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume);
}

void Engine::CSoundMgr::Set_BGMVolume(CHANNEL_TYPE eChannel, _float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume);
}

void Engine::CSoundMgr::Free(void)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter_begin = m_MapSound.begin();
	map<TCHAR*, FMOD_SOUND*>::iterator iter_end = m_MapSound.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		delete[] iter_begin->first;
		FMOD_Sound_Release(iter_begin->second);
	}

	m_MapSound.clear();

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}