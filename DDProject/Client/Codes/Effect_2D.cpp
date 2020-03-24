#include "stdafx.h"
#include "Effect_2D.h"
#include "Defines.h"

CEffect_2D::CEffect_2D(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CEffect(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_bRotating(FALSE)
	, m_bScaling(FALSE)
	, m_bUseScalingTime(FALSE)
	, m_bMaxScale(FALSE)
	, m_bScaleFadeOut(FALSE)
	, m_vecRotating(0.f, 0.f, 0.f)
	, m_fScalingDegree(0.f)
	, m_fScalingSpeed(0.f)
	, m_fMaxScale(0.f)
	, m_vecScalingTime(0.f, 0.f)
	, m_bScalingLoop(FALSE)
	, m_bUVAnimation(FALSE)
	, m_vecUV(1.f, 1.f)
	, m_vecUVAnimation(0.f, 0.f)
	, m_fUVAniSpeed(0.f)
	, m_fUVTime(0.f)
	, m_fInitScale(0.f)
	, m_bMoveDir(FALSE)
	, m_vMoveDIr(0.f, 0.f, 0.f)
	, m_fScaleFadeOutDegree(0.f)
	, m_bScalingLikeSpring(FALSE)
	, m_vScalingDegree(0.f, 0.f)
	, m_fMinScale(0.f)
{
}

CEffect_2D::CEffect_2D(const CEffect_2D& rhs)
	: Engine::CEffect(rhs)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pTextureCom(rhs.m_pTextureCom)
	, m_pBufferCom(rhs.m_pBufferCom)
	, m_bRotating(rhs.m_bRotating)
	, m_bScaling(rhs.m_bScaling)
	, m_bUseScalingTime(rhs.m_bUseScalingTime)
	, m_vecRotating(rhs.m_vecRotating)
	, m_bMaxScale(rhs.m_bMaxScale)
	, m_bScaleFadeOut(rhs.m_bScaleFadeOut)
	, m_fScalingDegree(rhs.m_fScalingDegree)
	, m_fScalingSpeed(rhs.m_fScalingSpeed)
	, m_fMaxScale(rhs.m_fMaxScale)
	, m_vecScalingTime(rhs.m_vecScalingTime)
	, m_bScalingLoop(rhs.m_bScalingLoop)
	, m_bUVAnimation(rhs.m_bUVAnimation)
	, m_vecUV(rhs.m_vecUV)
	, m_vecUVAnimation(rhs.m_vecUVAnimation)
	, m_fUVAniSpeed(rhs.m_fUVAniSpeed)
	, m_fUVTime(rhs.m_fUVTime)
	, m_fInitScale(rhs.m_fInitScale)
	, m_bMoveDir(rhs.m_bMoveDir)
	, m_vMoveDIr(rhs.m_vMoveDIr)
	, m_fScaleFadeOutDegree(rhs.m_fScaleFadeOutDegree)
	, m_bScalingLikeSpring(rhs.m_bScalingLikeSpring)
	, m_vScalingDegree(rhs.m_vScalingDegree)
	, m_fMinScale(rhs.m_fMinScale)
{
	m_pRendererCom->Add_Ref();
	m_pTextureCom->Add_Ref();
	m_pBufferCom->Add_Ref();
}

CEffect_2D::~CEffect_2D(void)
{
}

void CEffect_2D::Set_Scaling(_float fScalingDegree, _float fScalingSpeed)
{
	// 이 함수를 호출했다는것 = Scaling 옵션을 사용하겠다는 뜻
	// -> Mesh나 , 2DEffect나 Scaling이 있는데 , 최상위 부모로 올릴것인가 ? 훔
	// 귀찮아
	// 나중에 해야징 'ㅅ'

	m_bScaling = TRUE;
	m_fScalingDegree = fScalingDegree;
	m_fScalingSpeed = fScalingSpeed;

}

void CEffect_2D::Set_MaxScaling(_float fMaxScale, _float fInitScale, _bool bScaleLoop)
{
	m_bMaxScale = TRUE;
	m_bScalingLoop = bScaleLoop;
	m_fMaxScale = fMaxScale;
	m_fInitScale = fInitScale;
}

void CEffect_2D::Set_ScalingLikeSpring(_vec2 vScalingDegree, _float fMaxScale, _float fMinScale)
{
	m_bScaling = TRUE;
	m_bScalingLikeSpring = TRUE;
	m_vScalingDegree = vScalingDegree;
	m_fMaxScale = fMaxScale;
	m_fMinScale = fMinScale;
}

void CEffect_2D::Set_Rotating(_vec3 vRotating)
{
	m_bRotating = TRUE;
	m_vecRotating = _vec3(D3DXToRadian(vRotating.x), D3DXToRadian(vRotating.y), D3DXToRadian(vRotating.z));
}

void CEffect_2D::Set_UVAnimation(_vec2 vUVAni, _float fUVSpeed)
{
	m_bUVAnimation = TRUE;
	m_vecUVAnimation = vUVAni;
	m_fUVAniSpeed = fUVSpeed;
}

void CEffect_2D::Perform_Function(const float & fTimeDelta)
{
	if (m_bFadeOut && m_bUseTimeDead && m_fTime <= 1.f)
	{
		// 죽는시간이 1초 남구 , FadeOut을 수행한다면
		m_vColor.w -= m_fFadeOutSpeed * fTimeDelta;
	}

	if (m_bScaling)			// Use_Scaling
	{
		// 스케일을 증가하는 경우라면
		if (m_fScalingDegree >= 0.f)
		{
			if (m_bMaxScale && m_bScalingLoop && m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE).x >= m_fMaxScale)
			{
				// 맥스 스케일 넘으면 초기화 진행
				m_pTransformCom->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(m_fInitScale, m_fInitScale, 1.f));
				m_vColor.w = 1.f;
			}
			else if (!m_bScalingLikeSpring && m_bMaxScale && !m_bScalingLoop && m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE).x >= m_fMaxScale)
			{
				// 맥스 스케일 넘으면&&Scaling Loop를 안돈다면 ,
				m_bDead = TRUE; 
				// 죽인다
				return;
			}
		}
		else		// 감소하는 경우라면
		{
			if (m_bMaxScale && m_bScalingLoop && m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE).x <= m_fMaxScale)
			{
				// 맥스 스케일 넘으면 초기화 진행
				m_pTransformCom->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(m_fInitScale, m_fInitScale, 1.f));
				m_vColor.w = 1.f;
			}
			else if (!m_bScalingLikeSpring && m_bMaxScale && !m_bScalingLoop && m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE).x <= m_fMaxScale)
			{
				// 맥스 스케일 넘으면&&Scaling Loop를 안돈다면 ,
				m_bDead = TRUE;
				// 죽인다
				return;
			}
		}
		_float fScale_MulTime = m_fScalingDegree * m_fScalingSpeed * fTimeDelta;
		_vec3 vScale_MulTime = _vec3(fScale_MulTime, fScale_MulTime, 1.f);

		if (m_bMaxScale && m_bScaleFadeOut
			&& m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE).x >= m_fMaxScale - (m_fScalingDegree * m_fScalingSpeed))
		{
			//페이도 아웃
			m_vColor.w -= fTimeDelta * m_fScaleFadeOutDegree;
		}

		if (!m_bUseScalingTime && !m_bScalingLikeSpring)	// ScalingTime 사용 안할시,
		{
			m_pTransformCom->Change_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &vScale_MulTime);
		}
		// 아 코드 너무 더러워 ㅠㅠ

		if (m_bScalingLikeSpring)
		{
			_vec2 vScaling = m_vScalingDegree * fTimeDelta;
			_vec3 vFinalScaling = _vec3(vScaling.x, vScaling.y, 0.f);
			m_pTransformCom->Change_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &vFinalScaling);

			if (m_fMaxScale < m_pTransformCom->Get_Info(CTransform::INFO_SCALE).x)	// x기준으로 진행.............ㅠㅠ
				m_vScalingDegree *= -1.f;
			else if(m_fMinScale > m_pTransformCom->Get_Info(CTransform::INFO_SCALE).x)
				m_vScalingDegree *= -1.f;
		}
		
	}

	if (m_bRotating)
	{
		_vec3 fRotating_MulTime = m_vecRotating * fTimeDelta;

		m_pTransformCom->Change_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &fRotating_MulTime);

	}

	if (m_bMoveDir)
	{
		_vec3 vPos = m_vMoveDIr * fTimeDelta;
		m_pTransformCom->Change_Information(Engine::CTransform::INFORMATION::INFO_POSITION, &vPos);
	}

	if (m_bInstantFadeOut)
	{
		m_vColor.w -= m_fFadeOutSpeed * fTimeDelta;

		if (m_vColor.w <= 0.f)
		{
			m_bDead = TRUE;
			return;
		}
	}

}


void CEffect_2D::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);

	Engine::CEffect::Free();
}
