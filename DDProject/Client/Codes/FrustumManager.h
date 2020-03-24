#ifndef FrustumManager_h__
#define FrustumManager_h__

#include "Defines.h"

class CFrustumManager : public CBase
{
	DECLARE_SINGLETON(CFrustumManager)
private:
	explicit CFrustumManager();
	virtual ~CFrustumManager();
public:
	void Initialize_FrsutumMgr(void);

	void MakeFrustumPlane(D3DXMATRIX& matView, D3DXMATRIX& matProj);

	bool IsPointInFrustum(D3DXVECTOR3 vecPos);
	bool IsSphereInFrustum(D3DXVECTOR3 vecPos, float fRadius);

private:
	D3DXPLANE			 m_Plane[6];
	D3DXVECTOR3			 m_vecFrustumViewPort[8];
	D3DXVECTOR3			 m_vecFrustumWorld[8];
private:
	virtual void Free(void);
};

#endif