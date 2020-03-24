#include "Effect.h"
#include "Transform.h"
#include "DataManager.h"

Engine::CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pDataMgr(Engine::CDataManager::GetInstance())
	, m_bUseTimeDead(FALSE)
	, m_bFadeIn(FALSE)
	, m_bFadeOut(FALSE)
	, m_bInstantFadeOut(FALSE)
	, m_eBillState(NONE)
	, m_fTime(0.f)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
	, m_vDetailPos(0.f, 0.f, 0.f)
	, m_pmatFix(nullptr)
	, m_pmatParent(nullptr)
	, m_FinalPos(0.f, 0.f, 0.f)
	, m_bUseFinalPos(FALSE)
	, m_fFadeOutSpeed(1.f)
	, m_fSpeed(0.f)
	, m_bUseDir(FALSE)
	, m_vEffectDir(0.f, 0.f, 0.f)
	, m_bIsStartTimeControl(FALSE)
	, m_fStartTime(0.f)
	, m_eShaderPass(EFFECT_END)
	, m_bUseDotProduct(FALSE)
	, m_vTargetPos(0.f, 0.f, 0.f)
{
	m_pDataMgr->Add_Ref();
}

Engine::CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
	, m_pDataMgr(rhs.m_pDataMgr)
	, m_bUseTimeDead(rhs.m_bUseTimeDead)
	, m_bFadeIn(rhs.m_bFadeIn)
	, m_bFadeOut(rhs.m_bFadeOut)
	, m_bInstantFadeOut(rhs.m_bInstantFadeOut)
	, m_eBillState(rhs.m_eBillState)
	, m_fTime(rhs.m_fTime)
	, m_vColor(rhs.m_vColor)
	, m_vDetailPos(rhs.m_vDetailPos)
	, m_pmatFix(rhs.m_pmatFix)
	, m_pmatParent(rhs.m_pmatParent)
	, m_FinalPos(rhs.m_FinalPos)
	, m_bUseFinalPos(rhs.m_bUseFinalPos)
	, m_fFadeOutSpeed(rhs.m_fFadeOutSpeed)
	, m_fSpeed(rhs.m_fSpeed)
	, m_bUseDir(rhs.m_bUseDir)
	, m_vEffectDir(rhs.m_vEffectDir)
	, m_bIsStartTimeControl(rhs.m_bIsStartTimeControl)
	, m_fStartTime(rhs.m_fStartTime)
	, m_eShaderPass(rhs.m_eShaderPass)
	, m_bUseDotProduct(rhs.m_bUseDotProduct)
	, m_vTargetPos(rhs.m_vTargetPos)
{
	m_pDataMgr->Add_Ref();
}

Engine::CEffect::~CEffect(void)
{
}


void Engine::CEffect::Set_Information(Engine::CTransform::INFORMATION eInfo, const _vec3* pVector)
{
	if (m_pTransformCom == nullptr)
		return;
	m_pTransformCom->Set_Information(eInfo, pVector);
}

D3DXVECTOR3 Engine::CEffect::Get_Information(Engine::CTransform::INFORMATION eInfo)
{
	if (m_pTransformCom == nullptr)
		return D3DXVECTOR3(1.f, 1.f, 1.f);
	return m_pTransformCom->Get_Info(eInfo);
}

void Engine::CEffect::Revision_Pos(void)
{

	if (m_pmatFix != nullptr && m_pmatParent == nullptr)
	{	
		// Fix행렬 하나만 있을때
		_vec3 vRevisionPos(0.f, 0.f, 0.f);
		memcpy(&vRevisionPos, &m_pmatFix->m[3][0], sizeof(_vec3));
		vRevisionPos += m_vDetailPos;

		m_pTransformCom->Set_Information(Engine::CTransform::INFORMATION::INFO_POSITION, &vRevisionPos, TRUE);
	
		if (m_bUseDotProduct)	// 내적이 필요한 칭구들
		{
			_vec3 vLook, vDir, vRIght;
			memcpy(&vLook, &m_pTransformCom->Get_WorldMatrix()->m[2][0], sizeof(_vec3));
			//memcpy(&vRIght, &m_pTransformCom->Get_WorldMatrix()->m[0][0], sizeof(_vec3));
			D3DXVec3Normalize(&vLook, &vLook);
			//D3DXVec3Normalize(&vRIght, &vRIght);
			
			vLook = _vec3(1.f, 0.f, 0.f);
			D3DXVec3Normalize(&vLook, &vLook);

			vDir = m_vTargetPos - m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
			D3DXVec3Normalize(&vDir, &vDir);

			_float fAxisAngle = 0.f;
			fAxisAngle = acosf(D3DXVec3Dot(&vDir, &vLook));

			_vec3 vAxis;
			D3DXVec3Cross(&vAxis, &vLook, &vDir);

			_matrix matTemp;
			D3DXMatrixRotationAxis(&matTemp, &vAxis, fAxisAngle);

			_vec3 vSaveScale, vSavePos;
			vSaveScale = m_pTransformCom->Get_Info(CTransform::INFO_SCALE);
			memcpy(&vSavePos, &m_pTransformCom->Get_WorldMatrix()->m[3][0], sizeof(_vec3));

			_matrix matScaling;
			D3DXMatrixScaling(&matScaling, vSaveScale.x, vSaveScale.y, vSaveScale.z);

			//ZeroMemory(&matTemp.m[3][0], sizeof(D3DXVECTOR3));	// 위치 비움.
			memcpy(&matTemp.m[3][0], &m_pTransformCom->Get_Info(CTransform::INFO_POSITION), sizeof(_vec3));

			matTemp = matScaling * matTemp;
			//
			m_pTransformCom->Set_WorldMatrix(&matTemp);

			//_matrix matTemp;
			//_vec3 vYaw, vPitch, vRoll;		// Yaw = y, Pitch = x, Roll = z
			//_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);

			//memcpy(&vYaw, &m_pTransformCom->Get_WorldMatrix()->m[1][0], sizeof(_vec3));
			//memcpy(&vPitch, &m_pTransformCom->Get_WorldMatrix()->m[0][0], sizeof(_vec3));
			//memcpy(&vRoll, &m_pTransformCom->Get_WorldMatrix()->m[2][0], sizeof(_vec3));

			//_vec3 vDir = m_vTargetPos - vPos;
			//D3DXVec3Normalize(&vDir, &vDir);

			//_float fYawAngle = D3DXVec3Dot(&vDir, &vYaw);

			//fYawAngle = acosf(fYawAngle);
			//_float fPitchAngle = D3DXVec3Dot(&vDir, &vPitch);
			//fPitchAngle = acosf(fPitchAngle);
			//_float fRollAngle = D3DXVec3Dot(&vDir, &vRoll);
			//fRollAngle = acosf(fRollAngle);

			//D3DXMatrixRotationYawPitchRoll(&matTemp, fYawAngle, fPitchAngle, fRollAngle);
			//
			//ZeroMemory(&matTemp.m[3][0], sizeof(D3DXVECTOR3));	// 위치 비움.
			//memcpy(&matTemp.m[3][0], &m_pTransformCom->Get_Info(CTransform::INFO_POSITION), sizeof(_vec3));

			//m_pTransformCom->Set_WorldMatrix(&matTemp);

			//// 망했어
			//// 응 안해

			//_matrix matTemp;
			//_vec3 vEye, vUp;
			//memcpy(&vUp, &m_pTransformCom->Get_WorldMatrix()->m[1][0], sizeof(_vec3));
			//vEye = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
			//D3DXMatrixLookAtLH(&matTemp, &vEye, &m_vTargetPos, &vUp);

			//// 회전값을 따오자
			//_vec3 vDir = m_vTargetPos - vEye;
			//D3DXVec3Normalize(&vDir, &vDir);

			//_float fAngle = D3DXVec3Dot(&vDir, &vUp);
			//fAngle = acosf(fAngle);

			//D3DXMatrixRotationAxis(&matTemp, &vDir, -fAngle);
			//
			//ZeroMemory(&matTemp.m[3][0], sizeof(D3DXVECTOR3));	// 위치 비움.
			//memcpy(&matTemp.m[3][0], &m_pTransformCom->Get_Info(CTransform::INFO_POSITION), sizeof(_vec3));

			//m_pTransformCom->Set_WorldMatrix(&matTemp);
			////m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(90.f), D3DXToRadian(90.f)), TRUE);
		}
	}

	if (m_pmatFix != nullptr && m_pmatParent != nullptr)
	{
		// Fix행렬 + 부모행렬을 가지는 이펙트
		_matrix TempMat = (*m_pmatFix) *  (*m_pmatParent);

		_vec3 vRevisionPos(0.f, 0.f, 0.f);
		memcpy(&vRevisionPos, &TempMat.m[3][0], sizeof(_vec3));
		vRevisionPos += m_vDetailPos;

		m_pTransformCom->Set_Information(Engine::CTransform::INFORMATION::INFO_POSITION, &vRevisionPos);
		
		if (m_bUseDotProduct)	// 내적이 필요한 칭구들
		{
			cout << "탐?" << endl;
		}
	}

}

void Engine::CEffect::Bill_Perform(BILL_STATE BillState)
{

	switch (BillState)
	{
	case NONE:			// 빌보드 X
		return;
	case BILL_ALL:		// 모든 축 빌보드
	{
		// 뷰행렬의 역행렬(위치뺀)을 구해놓음.
		_matrix		matBill;
		D3DXMatrixIdentity(&matBill);

		matBill = m_pDataMgr->Get_ViewInverseMatrix();
		ZeroMemory(&matBill.m[3][0], sizeof(D3DXVECTOR3));

		// 원래 있던 Transform의 정보
		_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
		_vec3 vScale = m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE);
		_vec3 vRot = m_pTransformCom->Get_Info(Engine::CTransform::INFO_ANGLE);

		_matrix	matScale, matRotZ, matTrans;

		D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);
		D3DXMatrixRotationZ(&matRotZ, vRot.z);
		D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);

		matBill = matScale *  matRotZ * matBill * matTrans;

		m_pTransformCom->Set_WorldMatrix(&(matBill));
		return;
	}
	case BILL_Y:		// Y축만 빌보드
	{
		_matrix		matBill;
		D3DXMatrixIdentity(&matBill);

		_matrix		matView;
		matView = m_pDataMgr->Get_ViewMatrix();

		matBill._11 = matView._11;
		matBill._13 = matView._13;
		matBill._31 = matView._31;
		matBill._33 = matView._33;

		D3DXMatrixInverse(&matBill, NULL, &matBill);
		ZeroMemory(&matBill.m[3][0], sizeof(D3DXVECTOR3));

		// 원래 있던 Transform의 정보
		_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
		_vec3 vScale = m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE);
		_vec3 vRot = m_pTransformCom->Get_Info(Engine::CTransform::INFO_ANGLE);

		_matrix	matScale, matRotZ, matTrans;

		D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);
		D3DXMatrixRotationZ(&matRotZ, vRot.z);
		D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);

		matBill = matScale *  matRotZ * matBill * matTrans;

		m_pTransformCom->Set_WorldMatrix(&(matBill));
		return;
	}
	}

}


void Engine::CEffect::Set_OriginPos(_vec3 OriginPos)
{
	if (m_pTransformCom == nullptr)
		return;

	_vec3 vAdjustPos = OriginPos + m_vDetailPos;

	m_pTransformCom->Set_Information(Engine::CTransform::INFORMATION::INFO_POSITION, &vAdjustPos);
}

void Engine::CEffect::Toward_FinalPos(const float & fTimeDelta)
{
	if (!m_bUseFinalPos && !m_bUseDir)
		return;

	if (m_bUseFinalPos)
	{
		_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFORMATION::INFO_POSITION);

		_vec3 vDir = m_FinalPos - vPos;
		D3DXVec3Normalize(&vDir, &vDir);

		vPos = vPos + vDir * fTimeDelta * m_fSpeed;
		m_pTransformCom->Set_Information(Engine::CTransform::INFO_POSITION, &vPos);

		// 최종 목적지와 지금 Pos를 비교한다.
		_float fDistance = D3DXVec3Length(&(m_FinalPos - vPos));

		if (6.5f > fDistance)		// 일정 거리로 가까워지면
		{
			m_bDead = TRUE;
			return;
		}
		return;
	}
	if (m_bUseDir)
	{
		_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFORMATION::INFO_POSITION);

		vPos = vPos + m_vEffectDir * fTimeDelta * m_fSpeed;
		m_pTransformCom->Set_Information(Engine::CTransform::INFO_POSITION, &vPos);

		//// 최종 목적지와 지금 Pos를 비교한다.
		//_float fDistance = D3DXVec3Length(&(m_FinalPos - vPos));

		//if (6.5f > fDistance)		// 일정 거리로 가까워지면
		//{
		//	m_bDead = TRUE;
		//	return;
		//}
		return;
	}

	return;
}

void Engine::CEffect::Free(void)
{
	Safe_Release(m_pDataMgr);
	CGameObject::Free();
}
