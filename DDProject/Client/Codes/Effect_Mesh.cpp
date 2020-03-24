#include "stdafx.h"
#include "Effect_Mesh.h"
#include "Defines.h"

CEffect_Mesh::CEffect_Mesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CEffect(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pStaticMeshCom(nullptr)
	, m_bRotating(FALSE)
	, m_bUseRotatingTime(FALSE)
	, m_bScaling(FALSE)
	, m_bUseScalingTime(FALSE)
	, m_bUVAnimation(FALSE)
	, m_bMaxScale(FALSE)
	, m_bScaleFadeOut(FALSE)
	, m_bAlphaUp(FALSE)
	, m_vecRotating(0.f, 0.f, 0.f)
	, m_vecRotatingTime(0.f, 0.f)
	, m_fScalingDegree(0.f)
	, m_fScalingSpeed(0.f)
	, m_fMaxScale(0.f)
	, m_vecScalingTime(0.f, 0.f)
	, m_vecUV(1.f, 1.f)
	, m_vecUVAnimation(0.f, 0.f)
	, m_fUVAniSpeed(0.f)
	, m_iIndexPass(0)
	, m_fUVTime(0.f)
	, m_bScalingLoop(TRUE)
	, m_fScaleFadeOutDegree(0.f)
	, m_bFlicker(FALSE)
	, m_fFlickerDegree(1.f)
	, m_bScalingLikeSpring(FALSE)
	, m_vScalingDegree(0.f, 0.f, 0.f)
	, m_fMinScale(0.f)
	, m_bInitRotationRand(FALSE)
{
}

CEffect_Mesh::CEffect_Mesh(const CEffect_Mesh& rhs)
	: Engine::CEffect(rhs)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pStaticMeshCom(rhs.m_pStaticMeshCom)
	, m_bRotating(rhs.m_bRotating)
	, m_bUseRotatingTime(rhs.m_bUseRotatingTime)
	, m_bScaling(rhs.m_bScaling)
	, m_bUseScalingTime(rhs.m_bUseScalingTime)
	, m_bUVAnimation(rhs.m_bUVAnimation)
	, m_vecRotating(rhs.m_vecRotating)
	, m_vecRotatingTime(rhs.m_vecRotatingTime)
	, m_bMaxScale(rhs.m_bMaxScale)
	, m_bScaleFadeOut(rhs.m_bScaleFadeOut)
	, m_fScalingDegree(rhs.m_fScalingDegree)
	, m_fScalingSpeed(rhs.m_fScalingSpeed)
	, m_fMaxScale(rhs.m_fMaxScale)
	, m_vecScalingTime(rhs.m_vecScalingTime)
	, m_vecUV(rhs.m_vecUV)
	, m_vecUVAnimation(rhs.m_vecUVAnimation)
	, m_fUVAniSpeed(rhs.m_fUVAniSpeed)
	, m_iIndexPass(rhs.m_iIndexPass)
	, m_fUVTime(rhs.m_fUVTime)
	, m_bAlphaUp(rhs.m_bAlphaUp)
	, m_bScalingLoop(rhs.m_bScalingLoop)
	, m_fScaleFadeOutDegree(rhs.m_fScaleFadeOutDegree)
	, m_bFlicker(rhs.m_bFlicker)
	, m_fFlickerDegree(rhs.m_fFlickerDegree)
	, m_bScalingLikeSpring(rhs.m_bScalingLikeSpring)
	, m_vScalingDegree(rhs.m_vScalingDegree)
	, m_fMinScale(rhs.m_fMinScale)
	, m_bInitRotationRand(rhs.m_bInitRotationRand)
{
	m_pRendererCom->Add_Ref();
	m_pStaticMeshCom->Add_Ref();

}

CEffect_Mesh::~CEffect_Mesh(void)
{
}

void CEffect_Mesh::Set_Scaling(_float fScalingDegree, _float fScalingSpeed)
{
	m_bScaling = TRUE;
	m_fScalingDegree = fScalingDegree;
	m_fScalingSpeed = fScalingSpeed;
}

void CEffect_Mesh::Set_ScalingLikeSpring(_vec3 vScalingDegree, _float fMaxScale, _float fMinScale)
{
	m_bScaling = TRUE;
	m_bScalingLikeSpring = TRUE;
	m_vScalingDegree = vScalingDegree;
	m_fMaxScale = fMaxScale;
	m_fMinScale = fMinScale;
}

void CEffect_Mesh::Perform_Function(const float & fTimeDelta)
{

	if (m_pmatFix != nullptr || m_pmatParent != nullptr)
		Revision_Pos();		// &_matrix사용해서 만들어진 이펙트의 경우에, matrix의 Pos값을 계속 따라감 .
	
	if (m_bRotating)			// Use_Rotating
	{
		_vec3 Rot_MulTime = m_vecRotating * fTimeDelta;

		if (!m_bUseRotatingTime)	// RotatingTime 이용 안할시,
		{
			m_pTransformCom->Rotation(CTransform::ANGLE_X, D3DXToRadian(Rot_MulTime.x));
			m_pTransformCom->Rotation(CTransform::ANGLE_Y, D3DXToRadian(Rot_MulTime.y));
			m_pTransformCom->Rotation(CTransform::ANGLE_Z, D3DXToRadian(Rot_MulTime.z));
		}
		else if (m_bUseRotatingTime && 0.f <= m_vecRotatingTime.y)	// 시간 이용중이면서, 아직 도는시간이 남았으면
		{
			m_pTransformCom->Rotation(CTransform::ANGLE_X, D3DXToRadian(Rot_MulTime.x));
			m_pTransformCom->Rotation(CTransform::ANGLE_Y, D3DXToRadian(Rot_MulTime.y));
			m_pTransformCom->Rotation(CTransform::ANGLE_Z, D3DXToRadian(Rot_MulTime.z));

			m_vecRotatingTime.y -= fTimeDelta;	 // 빼준다.
		}
	}

	if (m_bScaling)			// Use_Scaling
	{
		// 스케일을 증가하는 경우라면
		if (m_fScalingDegree >= 0.f)
		{
			if (!m_bScalingLikeSpring && m_bMaxScale && m_bScalingLoop && m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE).x >= m_fMaxScale)
			{
				// 맥스 스케일 넘으면 초기화 진행
				m_pTransformCom->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.f, 0.f, 0.f));
				m_vColor.w = 1.f;
			}
			else if (!m_bScalingLikeSpring && m_bMaxScale && !m_bScalingLoop && m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE).x >= m_fMaxScale)
			{
				// 맥스 스케일 넘으면&&Scaling Loop를 안돈다면 ,
				m_bDead = TRUE;
				// 죽인다
			}
		}
		//else		// 감소하는 경우라면
		//{
		//	if (m_bMaxScale && m_bScalingLoop && m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE).x <= m_fMaxScale)
		//	{
		//		// 맥스 스케일 넘으면 초기화 진행
		//		m_pTransformCom->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(m_fInitScale, m_fInitScale, 1.f));
		//		m_vColor.w = 1.f;
		//	}
		//}
		_float fScale_MulTime = m_fScalingDegree * m_fScalingSpeed * fTimeDelta;
		_vec3 vScale_MulTime = _vec3(fScale_MulTime, fScale_MulTime, fScale_MulTime);

		if (m_bMaxScale && m_bScaleFadeOut
			&& m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE).x >= m_fMaxScale - (m_fScalingDegree * m_fScalingSpeed))
		{
			//페이도 아웃
			m_vColor.w -= fTimeDelta * m_fScaleFadeOutDegree;
		}

		if (!m_bScalingLikeSpring && !m_bUseScalingTime)	// ScalingTime 사용 안할시,
		{
			m_pTransformCom->Change_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &vScale_MulTime);
		}
		else if (m_bUseScalingTime && 0.f <= m_vecScalingTime.y)
		{
			m_pTransformCom->Change_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &vScale_MulTime);

			m_vecScalingTime.y -= fTimeDelta;	// 빼준다.
		}

		if (m_bScalingLikeSpring)
		{
			_vec3 vScaling = m_vScalingDegree * fTimeDelta;
			_vec3 vFinalScaling = _vec3(vScaling.x, vScaling.y, vScaling.z);
			m_pTransformCom->Change_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &vFinalScaling);

			if (m_fMaxScale < m_pTransformCom->Get_Info(CTransform::INFO_SCALE).x)	// x기준으로 진행.............ㅠㅠ!
				m_vScalingDegree *= -1.f;
			else if (m_fMinScale > m_pTransformCom->Get_Info(CTransform::INFO_SCALE).x)
				m_vScalingDegree *= -1.f;
		}

	}

}

void CEffect_Mesh::Perform_Flicker(const float & fTimeDelta)
{
	float fAlpha = m_vColor.w;

	if (m_bAlphaUp && fAlpha >= 1.f)		// Alpha를 올리는 상태인데 Alpha가 1이상
		m_bAlphaUp = FALSE;	// 알파 내려줌
	else if (!m_bAlphaUp && fAlpha <= 0.f)	// Alpha를 내리는 상태인데 Alpha가 0이하
		m_bAlphaUp = TRUE;	// 알파 올려줌.

	if (m_bAlphaUp)
		m_vColor.w += fTimeDelta * m_fFlickerDegree;
	else
		m_vColor.w -= fTimeDelta * m_fFlickerDegree;
}

void CEffect_Mesh::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pStaticMeshCom);

	Engine::CEffect::Free();
}
