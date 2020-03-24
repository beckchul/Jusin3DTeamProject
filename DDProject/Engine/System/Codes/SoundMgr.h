#pragma once
#include "Engine_Defines.h"

//// ���� ����� ���� fmod ���̺귯�� �߰�.
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
	// Cam��ġ, Sound�� �� ��ġ, Sound ����, Sound ����, ���带 1���� �÷��� �ϰ� ������(�̹� ���尡 ������̸� ���� ����)
	void Play_Sound(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, _vec3 vecCamEye, _vec3 vecSoundPos, _float fRange, _int iSoundNum = 1, _bool bPlayOnce = false); 
	void Play_RandomSound(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, int iNum, _float fVolume = 1.f); // 0~iNum ���� �� �������� ���
	bool IsPlaying(CHANNEL_TYPE eChannel); // �ش� ä���� Play������ Ȯ��

private:
	FMOD_SYSTEM*				m_pSystem;
	FMOD_CHANNEL*				m_pChannel[CHANNEL_END];

	map<TCHAR*, FMOD_SOUND*>	m_MapSound;
private:
	virtual void Free(void);
};

END
