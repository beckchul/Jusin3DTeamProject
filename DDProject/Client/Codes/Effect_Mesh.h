
#ifndef Effect_Mesh_h__
#define Effect_Mesh_h__

#include "Effect.h"
#include "Defines.h"

class CEffect_Mesh : public Engine::CEffect
{
public:
	explicit	 CEffect_Mesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_Mesh(const CEffect_Mesh& rhs);
	virtual	~CEffect_Mesh(void);

public:
	virtual CEffect_Mesh* Clone(void) PURE;

public:
	void Set_UVAnimation(_vec2 UVAnimation, _float fUVSpeed) { m_bUVAnimation = TRUE; m_vecUVAnimation = UVAnimation; m_fUVAniSpeed = fUVSpeed; }
	void Set_UV(_vec2 vUV) { m_vecUV = vUV; }
	void Set_Scaling(_float fScalingDegree, _float fScalingSpeed);
	void Set_MaxScaling(_float fMaxScaling, _bool bScaleLoop) { m_bMaxScale = TRUE; m_bScalingLoop = bScaleLoop; m_fMaxScale = fMaxScaling; }
	void Set_ScalingFadeOut(_float fFadeOutDegree = 1.f) { m_bScaleFadeOut = TRUE; m_fScaleFadeOutDegree = fFadeOutDegree; }
	void Set_Rotating(_vec3 vRotation) { m_bRotating = TRUE; m_vecRotating = vRotation; }
	void Set_FlickerDegree(_float fFlickerDegree) { m_bFlicker = TRUE; m_fFlickerDegree = fFlickerDegree; }
	void Set_ScalingLikeSpring(_vec3 vScalingDegree, _float fMaxScale, _float fMinScale);

	void Set_InitRotationRand(void) { m_bInitRotationRand = TRUE; }

protected:
	void Perform_Function(const float & fTimeDelta);		// 기능 수행
	void Perform_Flicker(const float & fTimeDelta);		// 알파 사용해서 깜빡거림.

protected:
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CStaticMesh*		m_pStaticMeshCom = nullptr;

protected:	
	_bool		m_bRotating;		
	_bool		m_bUseRotatingTime;

	_bool		m_bScaling;
	_bool		m_bMaxScale;
	_bool		m_bUseScalingTime;
	_bool		m_bScaleFadeOut;

	_bool		m_bUVAnimation;
	_bool		m_bAlphaUp;
	_bool		m_bScalingLoop;
	_bool		m_bScalingLikeSpring;	// 늘었다, 줄었다..ㅠㅠ

	_bool		m_bFlicker;
	_bool		m_bInitRotationRand;	// 첫 생성시 랜덤한 각도로 . .

protected:			// 속성에 따른 변수들
	_vec3		m_vecRotating;		
	_vec2		m_vecRotatingTime;

	_float		m_fScalingDegree;
	_float		m_fScalingSpeed;
	_float		m_fMaxScale;
	_float		m_fMinScale;
	_vec3		m_vScalingDegree;
	_float		m_fScaleFadeOutDegree;
	_vec2		m_vecScalingTime;

	_vec2		m_vecUV;			
	_vec2		m_vecUVAnimation;
	_float		m_fUVAniSpeed;
	_float		m_fFlickerDegree;

protected:
	_int			m_iIndexPass;		// RenderState 처리
	_float			m_fUVTime;		// UV흐르는 애니메이션을 위한 시간. ( 점진적 증가값을 필요로 함 )

protected:
	virtual void Free(void);


};

#endif // Effect_Mesh_h__