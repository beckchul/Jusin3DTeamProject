#ifndef Effect_RectParticle_h__
#define Effect_RectParticle_h__

#include "Effect.h"
#include "Defines.h"


class CEffect_RectParticle : public Engine::CEffect
{
protected:
	explicit CEffect_RectParticle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_RectParticle(const CEffect_RectParticle& rhs);
	virtual ~CEffect_RectParticle(void);

public:
	virtual CEffect_RectParticle* Clone(void);

public:
	virtual void Set_OriginPos(_vec3 OriginPos);
	HRESULT Set_DestPos(_vec3 DestPos);
	void Set_Dir(_vec3 vDir);									// Blood_Particle�� ��� .-> �� �Ƚ�~
	void Set_Speed(_float fSpeed);	
	void Set_RotZ(void) { m_bParticleRotZ = TRUE; }
	void Set_BubbleState(_float fStartTime, _float fDegree) { m_bBubble = TRUE;
	m_fBubbleDegree = fDegree;
	m_fBubbleTime = fStartTime;
	}
	void Set_DestBlendInv(void) { m_eShaderPass = EFFECT_RECTPARTICLE_INV; }
	void Set_SpreadExceptY(void) { m_ParticleInfo.bSpreadExceptY = TRUE; }
	void Set_LoopFalse(void) { m_ParticleInfo.bLoop = FALSE; }
	void Set_Smoke(_float fSmokeFadeSpeed) { m_bSmoke = TRUE;
	m_eShaderPass = CEffect::EFFECT_MESH_FOGMASK;
	m_fSmokeFadeSpeed = fSmokeFadeSpeed;}
	void Set_DefaultAlpha(_float fAlpha) { m_fDefaultAlpha = fAlpha; }
	void Set_FollowTrackingPath(void) { m_bFollowTrackingPath = TRUE; }
	void Set_FadeInfo(_vec2 vFadeInfo) { m_vFadeInfo = vFadeInfo; }
	void Set_FadeColor(_vec3 vColor, _float fColorFadeSpeed, _float fDegree) { m_bUseFadeColor = TRUE;
	m_vFadeColor = vColor; 
	m_fBubbleDegree = fDegree;
	}

public:
	virtual HRESULT	Ready_GameObject(void);
	virtual int			Update_GameObject(const float & fTimeDelta);
	virtual void			Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	virtual HRESULT	Add_Component(void);

private:
	HRESULT	Set_ConstantTable(LPD3DXEFFECT pEffect, _uint iIdex, _uint iTransIndex);

private:
	HRESULT	Initialize(Engine::PARTICLE_INFO ParticleInfo);
	void Add_Particle(void);
	void Reset_Particle(Engine::PARTICLE_ATT* pParticleInfo, Engine::CTransform* pTransform);
	HRESULT	Clean_Up(void);		// ���ؽ��� �̹����� ����.

private:	//Sort
	void	Compute_RectParticle_ViewZ(list<ALPHA_SORT*> plist);

private:	//Calc
	float Get_RandomFloat(float LowBound, float HighBound);
	void Get_RandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max);

private:
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CShader*					m_pShaderCom = nullptr;
	Engine::CTexture*					m_pTextureCom = nullptr;

	Engine::CRect_Texture*				m_pBufferCom = nullptr;
	Engine::CTransform**				m_pTransformArr;

private:
	Engine::CTexture*				m_pTextureMaskCom = nullptr;		// SmokeParticle����.
	Engine::CTexture*				m_pTextureFaderCom = nullptr;

private:
	list<_tchar*>							m_listHeapName;	// �̸� �����Ҵ��Ѱ� ������ . .

private:	// ���� ���� ��������. . . 
			// ������ ����ü�� ���ؼ� ������ ���Էε常 �ؾߴ�µ� ������
	_bool										m_bParticleRotZ;
	_bool										m_bBubble;
	_bool										m_bSmoke;
	_bool										m_bFollowTrackingPath;
	_bool										m_bUseFadeColor = FALSE;


private:
	vector<Engine::PARTICLE_ATT*>	m_vecParticle;			// ��ƼŬ �Ӽ� ����Ʈ
	Engine::PARTICLE_INFO				m_ParticleInfo;			// ��ƼŬ ����
	_float										m_fDelayCount;		// ���� ī��Ʈ
	_int										m_iCount;				// ���� �ɼ��� ���� ����
	_float										m_fGravityStartTime;
	_float										m_fSmokeFadeSpeed;
	_float										m_fBubbleDegree;
	_float										m_fBubbleTime;
	_float										m_fDefaultAlpha = 1.f;
	_int										m_iDeadParticleCount;	// �ӽ� ���� ó��
	_vec2										m_vFadeInfo;			// Fade ��������.
	_vec3										m_vFadeColor;
	_float										m_fFadeColorSpeed;


public:
	static CEffect_RectParticle* Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::PARTICLE_INFO ParticleInfo);

private:
	virtual void Free(void);

};

#endif // Effect_RectParticle_h__