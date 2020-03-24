#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagFrame_Derived : public D3DXFRAME
	{
		D3DXMATRIX		CombinedTransformationMatrix;
	}D3DXFRAME_DERIVED;

	typedef struct tagMeshContainer_Derived : public D3DXMESHCONTAINER
	{
		LPDIRECT3DTEXTURE9*			ppTextures;
		LPDIRECT3DTEXTURE9*			ppNormalTextures;
		LPDIRECT3DTEXTURE9*			ppSpecTextures;
		LPDIRECT3DTEXTURE9*			ppEmissiveTextures;
		DWORD						dwNumBone;
		LPD3DXMESH					pOriMesh;
		D3DXMATRIX*					pOffsetMatrices;
		D3DXMATRIX**				ppCombinedTransformationMatrices;
		D3DXMATRIX*					pRenderingMatrices;
		LPD3DXBUFFER				pBoneCombinationBuf;	// 하드웨어 스키닝 추가 변수
		DWORD						dwMartixPaletteCnt;
		TCHAR						szTextureName[256];
	}D3DXMESHCONTAINER_DERIVED;

	typedef struct tagVertexPoint
	{
		D3DXVECTOR3				vPos;
	}VTXPOS;
	const DWORD D3DFVF_VTXPOS = D3DFVF_XYZ;

	typedef struct tagVertexColor
	{
		D3DXVECTOR3				vPos;
		DWORD					dwColor;
	}VTXCOL;
	const DWORD VTXFVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertex_Texture
	{
		D3DXVECTOR3				vPosition;		
		D3DXVECTOR2				vTexUV;
	}VTXTEX;
	const DWORD D3DFVF_VTXTEX = D3DFVF_XYZ | D3DFVF_TEX1;

	typedef struct tagVertexTrail
	{
		D3DXVECTOR3				vPos;
		D3DXVECTOR2				vTexUV;
	}VTXTRAIL;
	const DWORD D3DFVF_VTXTRAIL = D3DFVF_XYZ | D3DFVF_TEX1;

	typedef struct tagVertex_ViewPort_Texture
	{
		D3DXVECTOR4				vPosition; // In.ViewPort == In.Client
		D3DXVECTOR2				vTexUV;
	}VTXVIEWTEX;
	const DWORD D3DFVF_VTXVIEWTEX = D3DFVF_XYZRHW | D3DFVF_TEX1;

	typedef struct tagVertex_Normal_Texture
	{
		D3DXVECTOR3				vPosition;
		D3DXVECTOR3				vNormal;
		D3DXVECTOR2				vTexUV;
	}VTXNORTEX;
	const DWORD D3DFVF_VTXNORTEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	typedef struct tagVertexCubeTex
	{
		D3DXVECTOR3				vPosition;
		D3DXVECTOR3				vTexUV;
	}VTXCUBETEX;
	const DWORD D3DFVF_VTXCUBETEX = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

	typedef struct tagParticle
	{
		D3DXVECTOR3			vPos;
		D3DCOLOR			Color;
	}VTXPARTICLE;
	//const DWORD PARTICLE_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	const DWORD D3DFVF_VTXPARTICLETEX = D3DFVF_XYZ | D3DFVF_DIFFUSE;



	typedef struct tagIndex16
	{
		WORD			_0, _1, _2;
	}INDEX16;

	typedef struct tagIndex32
	{
		DWORD			_0, _1, _2;
	}INDEX32;

	typedef struct tagNetData
	{
		char		szChatMessage[16];  // 채팅 메세지
		char		chKey;				// '72' 면 정상 메세지
		eNETMSG		eMessage;			// 주 메세지
		WORD		wSubMessage;		// 서브 메세지
		int			iIndex;				// 클라이언트 인덱스
		D3DXMATRIX  matWorld;			// 월드행렬
		int			iHighAnimation;     // 애니매이션(상체) 상태
		int			iLowAnimation;		// 애니매이션(하체) 상태
		int			eMainState;			// 플레이어 상태
		_vec3		vecPick;			// 피킹 지점
	}NETDATA;

	typedef struct tagAnimationInfo	// 애니메이션 구조체
	{
		tagAnimationInfo()
		{}

		tagAnimationInfo(_uint _uiAniIndex, _float _fAniSpeedFront, _float _fAniSpeedEnd, _float _fAniWeightFront, _float _fAniWeightEnd, _double _dMagicNumber, _double _dEndAniCount)
			: uiAniIndex(_uiAniIndex), fAniSpeedFront(_fAniSpeedFront), fAniSpeedEnd(_fAniSpeedEnd), fAniWeightFront(_fAniWeightFront), fAniWeightEnd(_fAniWeightEnd), dMagicNumber(_dMagicNumber), dEndAniCount(_dEndAniCount)
			{}

		_uint	uiAniIndex;
		_float	fAniSpeedFront;
		_float	fAniSpeedEnd;
		_float	fAniWeightFront;
		_float	fAniWeightEnd;
		_double	dMagicNumber;
		_double	dEndAniCount;
	}ANI_INFO;

	typedef struct tagNavMesh // 내비게이션 메쉬
	{
		tagNavMesh()
		{}

		tagNavMesh(D3DXVECTOR3 _Point_A, D3DXVECTOR3 _Point_B, D3DXVECTOR3 _Point_C, DWORD _dwOption, unsigned int _iIndex)
			:dwOption(_dwOption), iIndex(_iIndex)
		{
			vecPoint[0] = _Point_A;
			vecPoint[1] = _Point_B;
			vecPoint[2] = _Point_C;
		}

		// Point A,B,C
		D3DXVECTOR3 vecPoint[3];

		// Option
		DWORD		dwOption;

		// Index
		unsigned int iIndex;
	}NAVMESH;

	typedef struct tagObject
	{
		OBJECTID	eId;

		D3DXVECTOR3 vecPos;
		float		fAngle[ANGLE_END];
		D3DXVECTOR3 vecScale;
	}OBJECT;

	typedef struct tagParticleAttribute
	{
		_vec3					vPos;						// 위치
		_vec3					vVelocity;				// 방향
		_vec3					vAcc;						// 가속도
		_float					fLifeTime;				// 소멸까지의 시간
		_float					fAge;						// 파티클의 현 나이
		D3DXCOLOR		Color;					// 색상
		D3DXCOLOR		ColorFade;				// 색 퇴색 방법
		_bool					bIsDead;					// 죽으면 TRUE
		_float					fGravityStartTime;		// 중력 시작시간 설정(다 다를수도있으니가)

	}PARTICLE_ATT;

	typedef struct tagParticleInfo
	{
		////////// 변수
		_vec3		vOriginPos;				// 생성 위치
		_vec3		vDestPos;				// 목적 위치
		_vec3		vMinColor;				// 최소 색상
		_vec3		vMaxColor;				// 최대 색상
		_vec3		vPickColor;				// 지정 색상
		_vec3		vDir;						// 방향(사용시)
		_float		fEmitRate;				// 생성 속도
		_float		fSize;						// 사이즈
		_int		iMaxParticles;			// 최대 개수
		_float		fVelocity;				// float형 속도
		_float		fSpreadDegree;		// 생성 시 퍼지는 정도
		_float		fLife;						// 생명
		_int		iTextureIndex;			// 텍스쳐 인덱스

		////////// 속성
		_bool		bApplyDir;				// 방향 사용 
		_bool		bRandColor;			// 랜덤 색상 사용
		_bool		bCreateOnce;			// 한번에 생성 사용
		_bool		bGatherToSpot;		// 한 점(DestPos)에 모임
		_bool		bStartPosSpread;		// 생성시, 생성위치가 퍼짐	
		_bool		bFadeOut;				// FadeOut
		_bool		bAlphaBlend;			// False시 Test
		_bool		bLoop;					// Loop 사용 
		_bool		bGravity;					// 중력 사용
		_bool		bGravityTimeRand;	// 중력 시간 랜덤으로
		_bool		bSpreadExceptY;		// 위치 퍼지게할때 Y축빼구

	}PARTICLE_INFO;

	typedef struct tagObjectNameInfo
	{
		TCHAR szObjectName[128] = L"";
		TCHAR szObjectPath[128] = L"";
		TCHAR szObjectX[128] = L"";
	}OBJECT_NAME;

	typedef struct tagWaveCreateMonsterInfo
	{
		MONSTER_TYPE	eMonsterType;
		OBJECTID		eMonsterRoute;
		_vec3			vPosition;
		_float			fCreateTime;
		_int			iTotalMonsterCount = 0;
		_int			iRouteTotalMonsterCnt[5];		// 0~1, 2, 3~4, 5, 6~7 묶음
		_int			iRouteTotalGoblinCnt[5];
		_int			iRouteTotalDarkElfCnt[5];
		_int			iRouteTotalOrcCnt[5];
		_int			iRouteTotalKoboldCnt[5];
	}WAVECREATE_INFO;
}

#endif // Engine_Struct_h__