#ifndef Shader_h__
#define Shader_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader : public CComponent
{
private:
	explicit CShader(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShader(const CShader& rhs);
	virtual ~CShader(void);
public:
	LPD3DXEFFECT Get_EffectHandle(void) const {
		return m_pEffect;}
public:
	HRESULT Ready_Shader(const _tchar* pFilePath);
private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	LPD3DXEFFECT				m_pEffect;
	LPD3DXBUFFER				m_pErrBuffer;
public:
	virtual CComponent* Clone(void);
	static CShader* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath);
private:
	virtual void Free(void);

};

END

#endif // Shader_h__
