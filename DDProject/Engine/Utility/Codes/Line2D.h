
#ifndef Line2D_h__
#define Line2D_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CLine2D : public CBase
{
private:
	explicit CLine2D(void);
	virtual ~CLine2D(void);
public: // Getter
	_vec2 GetNormal(void) { return m_vNormal; }
public:
	HRESULT InitLine(const _vec3* pStartPoint, const _vec3* pEndPoint);
	bool CheckLinePass(const _vec2* pMoveEndPoint);
private:
	_vec2			m_vStartPoint;
	_vec2			m_vEndPoint;
	_vec2			m_vNormal;
public:
	static CLine2D* Create(const _vec3* pStartPoint, const _vec3* pEndPoint);
private:
	virtual void Free(void) final;
};

END

#endif // Line2D_h__