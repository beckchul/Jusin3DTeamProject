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
		LPD3DXBUFFER				pBoneCombinationBuf;	// �ϵ���� ��Ű�� �߰� ����
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
		char		szChatMessage[16];  // ä�� �޼���
		char		chKey;				// '72' �� ���� �޼���
		eNETMSG		eMessage;			// �� �޼���
		WORD		wSubMessage;		// ���� �޼���
		int			iIndex;				// Ŭ���̾�Ʈ �ε���
		D3DXMATRIX  matWorld;			// �������
		int			iHighAnimation;     // �ִϸ��̼�(��ü) ����
		int			iLowAnimation;		// �ִϸ��̼�(��ü) ����
		int			eMainState;			// �÷��̾� ����
		_vec3		vecPick;			// ��ŷ ����
	}NETDATA;

	typedef struct tagAnimationInfo	// �ִϸ��̼� ����ü
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

	typedef struct tagNavMesh // ������̼� �޽�
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
		_vec3					vPos;						// ��ġ
		_vec3					vVelocity;				// ����
		_vec3					vAcc;						// ���ӵ�
		_float					fLifeTime;				// �Ҹ������ �ð�
		_float					fAge;						// ��ƼŬ�� �� ����
		D3DXCOLOR		Color;					// ����
		D3DXCOLOR		ColorFade;				// �� ��� ���
		_bool					bIsDead;					// ������ TRUE
		_float					fGravityStartTime;		// �߷� ���۽ð� ����(�� �ٸ����������ϰ�)

	}PARTICLE_ATT;

	typedef struct tagParticleInfo
	{
		////////// ����
		_vec3		vOriginPos;				// ���� ��ġ
		_vec3		vDestPos;				// ���� ��ġ
		_vec3		vMinColor;				// �ּ� ����
		_vec3		vMaxColor;				// �ִ� ����
		_vec3		vPickColor;				// ���� ����
		_vec3		vDir;						// ����(����)
		_float		fEmitRate;				// ���� �ӵ�
		_float		fSize;						// ������
		_int		iMaxParticles;			// �ִ� ����
		_float		fVelocity;				// float�� �ӵ�
		_float		fSpreadDegree;		// ���� �� ������ ����
		_float		fLife;						// ����
		_int		iTextureIndex;			// �ؽ��� �ε���

		////////// �Ӽ�
		_bool		bApplyDir;				// ���� ��� 
		_bool		bRandColor;			// ���� ���� ���
		_bool		bCreateOnce;			// �ѹ��� ���� ���
		_bool		bGatherToSpot;		// �� ��(DestPos)�� ����
		_bool		bStartPosSpread;		// ������, ������ġ�� ����	
		_bool		bFadeOut;				// FadeOut
		_bool		bAlphaBlend;			// False�� Test
		_bool		bLoop;					// Loop ��� 
		_bool		bGravity;					// �߷� ���
		_bool		bGravityTimeRand;	// �߷� �ð� ��������
		_bool		bSpreadExceptY;		// ��ġ �������Ҷ� Y�໩��

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
		_int			iRouteTotalMonsterCnt[5];		// 0~1, 2, 3~4, 5, 6~7 ����
		_int			iRouteTotalGoblinCnt[5];
		_int			iRouteTotalDarkElfCnt[5];
		_int			iRouteTotalOrcCnt[5];
		_int			iRouteTotalKoboldCnt[5];
	}WAVECREATE_INFO;
}

#endif // Engine_Struct_h__