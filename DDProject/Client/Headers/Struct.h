#pragma once

#include "Engine_Defines.h"

#include "Transform.h"

USING(Engine)

typedef struct tagUnitInfo // 유닛(플레이어,몬스터,타워 등) 공통 정보
{
	tagUnitInfo()
	{}

	tagUnitInfo(_int _iHp, _int _iHpMax, _int _iMp, _int _iMpMax, _int _iAttPower, _bool _bDead, _float _fSpeedX, _float _fSpeedY, _float _fSpeedZ)
		:iHp(_iHp), iHpMax(_iHpMax), iMp(_iMp), iMpMax(_iMpMax), iAttPower(_iAttPower), bDead(_bDead), fSpeedX(_fSpeedX), fSpeedY(_fSpeedY), fSpeedZ(_fSpeedZ)
	{}

	_int	iHp;
	_int	iHpMax;
	_int	iMp;
	_int	iMpMax;
	_int	iAttPower;
	_bool	bDead;
	_float	fSpeedX;
	_float  fSpeedY;
	_float	fSpeedZ;
}INFO;

typedef struct tagPlayerInfo // 플레이어 정보
{
	tagPlayerInfo(int _iPlayerIdx, CTransform* _pTransform, _matrix* _pWeaponMatrix, PLAYER_TYPE _ePlayerType, PLAYER_MAIN_STATE _eMainState, _int _iPlayerScore, _int _iPlayerLevel, _float _fCameraAngle, INFO _tUnitInfo)
		:iPlayerIdx(_iPlayerIdx), pTransform(_pTransform), pWeaponMatrix(_pWeaponMatrix), ePlayerType(_ePlayerType), eMainState(_eMainState),
		iPlayerScore(_iPlayerScore), iPlayerLevel(_iPlayerLevel), fCameraAngle(_fCameraAngle), tUnitInfo(_tUnitInfo)
	{}

	_int				iPlayerIdx = -1;
	CTransform*			pTransform = nullptr;
	_matrix*			pWeaponMatrix = nullptr;
	PLAYER_TYPE			ePlayerType = PLAYER_TYPE_END;
	PLAYER_MAIN_STATE	eMainState = PLAYER_END;
	_int				iPlayerScore = 0;
	_int				iPlayerLevel = 1;
	float				fCameraAngle = 0.f;

	INFO				tUnitInfo;

}PLAYER_INFO;

typedef struct tagLastManInfo
{
	tagLastManInfo()
	{}
	tagLastManInfo(_int _iPlayerIdx, _vec3 _vecPos, _matrix* _pWorldMatrix, _float _fCameraAngle, INFO _tUnitInfo, MONSTER_MAIN_STATE _eMainState, _int _iRenderState)
		:iPlayerIdx(_iPlayerIdx), vecPos(_vecPos), pWorldMatrix(_pWorldMatrix), fCameraAngle(_fCameraAngle), tUnitInfo(_tUnitInfo), eMainState(_eMainState), iRenderState(_iRenderState)
	{}

	_int				iPlayerIdx = -1;

	_vec3				vecPos;
	_matrix*			pWorldMatrix;
	_float				fCameraAngle;

	INFO				tUnitInfo;
	MONSTER_MAIN_STATE	eMainState;

	_uint				uKillAI = 0;
	_uint				uKillPlayer = 0;

	_int				iRenderState = 0;
}LASTMAN_INFO;

typedef struct tagWaveInfo
{
	// Build, Combat
	PHASE_TYPE ePhaseType;

	// Cut-Scene 중인지
	bool	bCutScene = false;

	// 보스 등장
	bool	bBossExist = false;

	_int iCurrentWave = 1;
	_int iTotalWave = 6;
	
	_int iCurUnits = 0;
	_int iTotalUnits = 60;

	_int iBossHp = 0;
	
	_bool bWaveClear = false;
}WAVE_INFO;

typedef struct tagCamInfo
{
	_vec3 vecEye;
	_vec3 vecAt;

}CAM_INFO;

typedef struct tagFollowTarget
{
	MONSTER_TURN_CAHCK	eMonsterTurn;
	_float				fTargetLength;
	_float				fAttackRange;
	_float				fSearchLength;
	_bool				bTargetCheck;
	_bool				bTargetFllow;
	_vec3				vTargetDir;
}FOLLOW_TARGERT;

typedef struct tagTargetCheck
{
	_float			fAngle;
	_float			fRength;
	_bool			bTarget;
	_vec3			vTargetDir;
}TARGET_CHECK;

typedef struct tagFrame
{
	float fFrame;
	float fCount;
	float fMax;

	tagFrame(float _fFrame = 0, float _fCount = 0, float _fMax = 0)
		:fFrame(_fFrame)
		, fCount(_fCount)
		, fMax(_fMax)
	{}

}FRAME;

typedef struct tagAlphaSort	// Minkyeong
{
	_matrix* matWorld;
	_float		fViewZ;
	_uint		iIndex;
	tagAlphaSort(_matrix* _matWorld, _float _fViewZ, _uint _iIndex)
		: matWorld(_matWorld)
		, fViewZ(_fViewZ)
		, iIndex(_iIndex)
	{}
}ALPHA_SORT;