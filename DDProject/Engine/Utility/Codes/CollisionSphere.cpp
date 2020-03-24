#include "CollisionSphere.h"

USING(Engine)

CCollisionSphere::CCollisionSphere(LPDIRECT3DDEVICE9 pGraphicDev)
: CCollision(pGraphicDev)
, m_fRadius(20.f)
, m_pCollisionSphere(nullptr)
, m_pCollisionBoneMatrix(nullptr)
, m_bCheckCollision(true)
{
}

CCollisionSphere::CCollisionSphere(const CCollisionSphere & rhs)
: CCollision(rhs.m_pGraphicDev)
, m_pCollisionBoneMatrix(rhs.m_pCollisionBoneMatrix)
, m_pCollisionSphere(rhs.m_pCollisionSphere)
, m_matWorldCollision(rhs.m_matWorldCollision)
, m_fRadius(rhs.m_fRadius)
, m_bCheckCollision(rhs.m_bCheckCollision)
{
}

CCollisionSphere::~CCollisionSphere(void)
{
}

void CCollisionSphere::Create_Sphere(_float fRadius, const _matrix* pBoneMatrix, _float fScale)
{
	m_fRadius = fRadius;
	m_fScale = fScale;
	m_pCollisionBoneMatrix = pBoneMatrix;
	D3DXCreateSphere(m_pGraphicDev, m_fRadius, 20, 20, &m_pCollisionSphere, NULL);
}

int CCollisionSphere::Update_CollisionSphere(const _matrix* pWorldMatrix)
{
	if (nullptr == pWorldMatrix)
		return -1;

	m_matWorldCollision = *(m_pCollisionBoneMatrix) * (*pWorldMatrix);

	return 0;
}

void CCollisionSphere::Render_CollisionSphere(void)
{
	if (!m_bRender)
		return;
#ifdef _DEBUG
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorldCollision);

	m_pCollisionSphere->DrawSubset(0);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif
}

_bool CCollisionSphere::Check_Collision(CCollisionSphere* pTarget)
{
	if (nullptr == pTarget)
		return false;

	if (!pTarget->Get_CheckCollision())
		return false;

	_float	fRaidus = pTarget->Get_Radius();
	_matrix	matWorldCollision = pTarget->Get_WorldMatrix();

	_vec3 vPos, vPos2;
	memcpy(&vPos, &m_matWorldCollision.m[3][0], sizeof(_vec3));
	memcpy(&vPos2, &matWorldCollision.m[3][0], sizeof(_vec3));

	if (m_fRadius * m_fScale + fRaidus * pTarget->Get_Scale() < D3DXVec3Length(&(vPos - vPos2)))
		return false;

	return true;
}

_bool CCollisionSphere::Check_AuraCollision(CCollisionSphere * pTarget)
{
	if (nullptr == pTarget)
		return false;

	if (!pTarget->Get_CheckCollision())
		return false;

	_float	fRaidus = pTarget->Get_Radius();
	_matrix	matWorldCollision = pTarget->Get_WorldMatrix();

	_vec3 vPos, vPos2;
	memcpy(&vPos, &m_matWorldCollision.m[3][0], sizeof(_vec3));
	memcpy(&vPos2, &matWorldCollision.m[3][0], sizeof(_vec3));

	if (fRaidus * pTarget->Get_Scale() < D3DXVec3Length(&(vPos - vPos2)))
		return false;

	return true;
}

_bool CCollisionSphere::Push_Collision(CCollisionSphere* pTarget)
{
	if (nullptr == pTarget)
		return false;
	if (this == pTarget)
		return false;

	_float	fRaidus = pTarget->Get_Radius();
	_matrix	matWorldCollision = pTarget->Get_WorldMatrix();

	_vec3 vPos, vPos2;
	memcpy(&vPos, &m_matWorldCollision.m[3][0], sizeof(_vec3));
	memcpy(&vPos2, &matWorldCollision.m[3][0], sizeof(_vec3));

	if (m_fRadius * m_fScale + fRaidus * pTarget->Get_Scale() < D3DXVec3Length(&(vPos - vPos2)))
		return false;


	D3DXVec3Normalize(&m_vDir , &(vPos - vPos2));
	return true;
}

CComponent* CCollisionSphere::Clone(void)
{
	return new CCollisionSphere(*this);
}

CCollisionSphere* CCollisionSphere::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCollisionSphere*		pInstance = new CCollisionSphere(pGraphicDev);

	if (FAILED(pInstance->Ready_CollisionSphere()))
	{
		MSG_BOX(L"CCollisionSphere Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollisionSphere::Free(void)
{
	Safe_Release(m_pCollisionSphere);
	CCollision::Free();
}
