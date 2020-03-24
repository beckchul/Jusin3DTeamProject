
#ifndef Effect_2D_h__
#define Effect_2D_h__

#include "Effect.h"
#include "Defines.h"

class CEffect_2D : public Engine::CEffect
{
public:
	explicit	 CEffect_2D(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	 CEffect_2D(const CEffect_2D& rhs);
	virtual	~CEffect_2D(void);

public:
	virtual CEffect_2D* Clone(void) PURE;

public:
	void Set_Scaling(_float fScalingDegree, _float fScalingSpeed);
	void Set_MaxScaling(_float fMaxScale, _float fInitScale, _bool bScaleLoop);
	void Set_ScalingFadeOut(_float fFadeOutDegree = 1.f) { m_bScaleFadeOut = TRUE; m_fScaleFadeOutDegree = fFadeOutDegree; }
	void Set_ScalingLikeSpring(_vec2 vScalingDegree, _float fMaxScale, _float fMinScale);
	void Set_Rotating(_vec3 vRotating);
	void Set_UV(_vec2 vUV) { m_vecUV = vUV; }
	void Set_UVAnimation(_vec2 vUVAni, _float fUVSpeed);
	void Set_MoveDir(_vec3 vMoveDir) { m_bMoveDir = TRUE; m_vMoveDIr = vMoveDir; }

protected:
	void Perform_Function(const float & fTimeDelta);		// 기능 수행

protected:
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CTexture*					m_pTextureCom = nullptr;
	Engine::CRect_Texture*			m_pBufferCom = nullptr;

protected:
	_bool		m_bRotating;

	_bool		m_bScaling;
	_bool		m_bMaxScale;
	_bool		m_bUseScalingTime;
	_bool		m_bScaleFadeOut;
	_bool		m_bScalingLoop;	// 이거는 TRUE로 하면 계속 반복, FALSE면 한번 max에 도달 - 삭제
	_bool		m_bScalingLikeSpring;	// 늘었다, 줄었다..ㅠㅠ

	_bool		m_bMoveDir;


protected:			// 속성에 따른 변수들
	_vec3		m_vecRotating;

	_float		m_fScalingDegree;
	_float		m_fScalingSpeed;
	_float		m_fMaxScale;
	_float		m_fMinScale;
	_float		m_fInitScale;
	_vec2		m_vecScalingTime;
	_vec2		m_vScalingDegree;
	_float		m_fScaleFadeOutDegree = 1.f;

	_vec3		m_vMoveDIr;


protected:		// UVAnimation
	_bool		m_bUVAnimation;
	_vec2		m_vecUV;
	_vec2		m_vecUVAnimation;
	_float		m_fUVAniSpeed;
	_float		m_fUVTime;		// UV흐르는 애니메이션을 위한 시간. ( 점진적 증가값을 필요로 함 )


protected:
	virtual void Free(void);


};

#endif // CEffect_2D