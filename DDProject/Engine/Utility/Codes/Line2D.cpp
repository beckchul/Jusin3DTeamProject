#include "Line2D.h"

Engine::CLine2D::CLine2D(void)
{

}

Engine::CLine2D::~CLine2D(void)
{

}

HRESULT Engine::CLine2D::InitLine(const _vec3* pStartPoint, const _vec3* pEndPoint)
{
	m_vStartPoint = _vec2(pStartPoint->x, pStartPoint->z);
	m_vEndPoint = _vec2(pEndPoint->x, pEndPoint->z);

	_vec2		vDir = m_vEndPoint - m_vStartPoint;
	m_vNormal = _vec2(vDir.y * -1.f, vDir.x);
	D3DXVec2Normalize(&m_vNormal, &m_vNormal);

	return S_OK;
}

bool Engine::CLine2D::CheckLinePass(const _vec2* pMoveEndPoint)
{
	_vec2		vDir = *pMoveEndPoint - m_vStartPoint;

	float		fDotResult = D3DXVec2Dot(&vDir, &m_vNormal);

	//cout << fDotResult << endl;

	if (fDotResult > 0.f)
		return true;
	else
		return false;
}

Engine::CLine2D* Engine::CLine2D::Create(const _vec3* pStartPoint, const _vec3* pEndPoint)
{
	CLine2D* pLine = new CLine2D;
	if (FAILED(pLine->InitLine(pStartPoint, pEndPoint)))
		Safe_Release(pLine);

	return pLine;
}

void Engine::CLine2D::Free(void)
{
}



