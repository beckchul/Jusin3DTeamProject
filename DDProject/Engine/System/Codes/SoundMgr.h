#pragma once
#include "Engine_Defines.h"

//// 사운드 재생을 위한 fmod 라이브러리 추가.
#include "io.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_dsp.h"
#include "fmod_errors.h"

#pragma comment(lib, "fmodex_vc.lib")

BEGIN(Engine)

class ENGINE_DLL CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)
private:
	explicit CSoundMgr(void);
	virtual ~CSoundMgr(void);

public:
	void Initialize(void);
	void LoadSoundFile();
	void Play_Sound(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, _float fVolume = 1.f);
	void PlayBGM(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, _float fVolume = 1.f);
	void Set_BGMVolume(CHANNEL_TYPE eChannel, _float fVolume);
	void UpdateSound(void);
	void StopSound(CHANNEL_TYPE eChannel);
	void StopSoundAll(void);

public:
	// jsh
	// Cam위치, Sound가 난 위치, Sound 범위, Sound 갯수, 사운드를 1번만 플레이 하고 싶은지(이미 사운드가 재생중이면 덮지 않음)
	void Play_Sound(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, _vec3 vecCamEye, _vec3 vecSoundPos, _float fRange, _int iSoundNum = 1, _bool bPlayOnce = false); 
	void Play_RandomSound(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, int iNum, _float fVolume = 1.f); // 0~iNum 까지 중 랜덤으로 재생
	bool IsPlaying(CHANNEL_TYPE eChannel); // 해당 채널이 Play중인지 확인

private:
	FMOD_SYSTEM*				m_pSystem;
	FMOD_CHANNEL*				m_pChannel[CHANNEL_END];

	map<TCHAR*, FMOD_SOUND*>	m_MapSound;
private:
	virtual void Free(void);
};

END
