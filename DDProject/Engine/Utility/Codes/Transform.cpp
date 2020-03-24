#include "Transform.h"
#include "NavMgr.h"

USING(Engine)

Engine::CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphicDev)
: m_pGraphicDev(pGraphicDev)
, m_pParentMatrix(NULL)
{
	m_pGraphicDev->AddRef();

	ZeroMemory(m_vInformation, sizeof(_vec3) * INFO_END);

	m_vInformation[INFO_SCALE] = _vec3(1.f, 1.f, 1.f);
}

Engine::CTransform::CTransform(const CTransform& rhs)
: m_pGraphicDev(rhs.m_pGraphicDev)
, m_matWorld(rhs.m_matWorld)
, m_pParentMatrix(rhs.m_pParentMatrix)
{
	m_pGraphicDev->AddRef();
	memcpy(m_vInformation, rhs.m_vInformation, sizeof(_vec3) * INFO_END);
}

Engine::CTransform::~CTransform(void)
{

}

void Engine::CTransform::Set_Information(INFORMATION eInfo, const _float& fX, const _float& fY, const _float& fZ)
{
	m_vInformation[eInfo].x = fX;
	m_vInformation[eInfo].y = fY;
	m_vInformation[eInfo].z = fZ;
}

void Engine::CTransform::Set_Information(INFORMATION eInfo, const _vec3* pVector, bool bUpdate)
{
	m_vInformation[eInfo] = *pVector;

	if (bUpdate)
		Update_Component();
}

void CTransform::Change_Information(INFORMATION eInfo, const _vec3 * pVector)
{
	m_vInformation[eInfo] += *pVector;
}

void CTransform::Set_WorldMatrix(const _matrix * matWorld)
{
	m_matWorld = *matWorld;
}

void CTransform::Change_WorldMatrix(const _vec3* pVector) // Transform 정보를 바로 바꿔줌
{
	m_matWorld.m[3][0] += pVector->x;
	m_matWorld.m[3][1] += pVector->y;
	m_matWorld.m[3][2] += pVector->z;
}

void Engine::CTransform::Set_ParentMatrix(_matrix* pParentMatrix)
{
	m_pParentMatrix = pParentMatrix;
}

void Engine::CTransform::SetUp_OnGraphicDev(void)
{
	if(nullptr == m_pGraphicDev)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
}

void Engine::CTransform::SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName)
{
	if (nullptr == pEffect)
		return;

	pEffect->SetMatrix(pConstantName, &m_matWorld);
}

// 미리 행렬을 곱해서 전달하면 더 빠르다.
void Engine::CTransform::SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName, _matrix matView, _matrix matProj)
{
	if(nullptr == pEffect)
		return;

	_matrix matWorldView, matWorldViewProj;

	D3DXMatrixMultiply(&matWorldView, &m_matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &matProj);

	pEffect->SetMatrix("g_matWorld", &m_matWorld); // 월드행렬이 따로 필요할 수 있다.
	pEffect->SetMatrix(pConstantName, &matWorldViewProj);
}

void Engine::CTransform::SetRotation(ANGLE eType, const _float& fAngleTimeDelta)
{
	*((_float*)&m_vInformation[INFO_ANGLE] + eType) = fAngleTimeDelta;
}

HRESULT Engine::CTransform::Ready_Component(void)
{
	D3DXMatrixIdentity(&m_matWorld);

	return S_OK;
}

int Engine::CTransform::Update_Component(void)
{
	_matrix				matScale, matRotX, matRotY, matRotZ, matTrans;

	D3DXMatrixScaling(&matScale, m_vInformation[INFO_SCALE].x, m_vInformation[INFO_SCALE].y, m_vInformation[INFO_SCALE].z);
	D3DXMatrixRotationX(&matRotX, m_vInformation[INFO_ANGLE].x);
	D3DXMatrixRotationY(&matRotY, m_vInformation[INFO_ANGLE].y);
	D3DXMatrixRotationZ(&matRotZ, m_vInformation[INFO_ANGLE].z);
	D3DXMatrixTranslation(&matTrans, m_vInformation[INFO_POSITION].x, m_vInformation[INFO_POSITION].y, m_vInformation[INFO_POSITION].z);

	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

	if (nullptr != m_pParentMatrix)
		m_matWorld *= *m_pParentMatrix;

	return 0;
}

DWORD Engine::CTransform::Go_Straight(const _float& fSpeedTimeDelta, DWORD dwIndx, bool bDropoAble)
{
	_vec3			vDir = *((_vec3*)&m_matWorld.m[2][0]);
	D3DXVec3Normalize(&vDir, &vDir);

	return Engine::CNavMgr::GetInstance()->MoveOnNavMesh(&m_vInformation[INFO_POSITION], &(vDir * fSpeedTimeDelta), dwIndx, bDropoAble);
}

void Engine::CTransform::Go_FlyStraight(const _float& fSpeedTimeDelta)
{
	_vec3			vDir = *((_vec3*)&m_matWorld.m[2][0]);
	D3DXVec3Normalize(&vDir, &vDir);

	m_vInformation[INFO_POSITION] += vDir * fSpeedTimeDelta;
}

DWORD CTransform::Go_Right(const _float & fSpeedTimeDelta, DWORD dwIndx, bool bDropoAble)
{
	_vec3			vDir = *((_vec3*)&m_matWorld.m[0][0]);
	D3DXVec3Normalize(&vDir, &vDir);

	return Engine::CNavMgr::GetInstance()->MoveOnNavMesh(&m_vInformation[INFO_POSITION], &(vDir * fSpeedTimeDelta), dwIndx, bDropoAble);
}
void Engine::CTransform::Go_FlyRight(const _float& fSpeedTimeDelta)
{
	_vec3			vDir = *((_vec3*)&m_matWorld.m[0][0]);
	D3DXVec3Normalize(&vDir, &vDir);

	m_vInformation[INFO_POSITION] += vDir * fSpeedTimeDelta;
}

void CTransform::Go_Down(const _float & fSpeedTimeDelta)
{
	m_vInformation[INFO_POSITION] += _vec3(0.f, 1.f, 0.f) * fSpeedTimeDelta;
}

void Engine::CTransform::Rotation(ANGLE eType, const _float& fAngleTimeDelta)
{
	*((_float*)&m_vInformation[INFO_ANGLE] + eType) += fAngleTimeDelta;
}

CComponent* Engine::CTransform::Clone(void)
{
	return new CTransform(*this);
}

CTransform* Engine::CTransform::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTransform* pInstance = new CTransform(pGraphicDev);

	if(FAILED(pInstance->Ready_Component()))
	{
		MSG_BOX(L"CTransform Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::CTransform::Free(void)
{
	Engine::Safe_Release(m_pGraphicDev);

}


