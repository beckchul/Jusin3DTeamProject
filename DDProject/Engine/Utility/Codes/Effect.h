#ifndef Effect_h__
#define Effect_h__

#include "GameObject.h"

BEGIN(Engine)

class CDataManager;

class ENGINE_DLL CEffect abstract : public CGameObject
{
public:
	enum BILL_STATE { NONE, BILL_Y, BILL_ALL, BILL_END };
	enum SHADER_EFFECT_PASS
	{
		EFFECT_2D_SOFTBLEND,
		EFFECT_2D_ALPHABLEND,
		EFFECT_ALPHABLEND_NO_Z,
		EFFECT_RECTPARTICLE,
		EFFECT_RECTPARTICLE_INV,
		EFFECT_MESH_ALPHABLEND,
		EFFECT_MESH_ALPHABLEND_CULLNONE,	// Cullmode=NONE
		EFFECT_MESH_ALPHATEST,
		EFFECT_MESH_MASK,
		EFFECT_MESH_FOGMASK,
		EFFECT_DECAL,
		EFFECT_MESH_PROJECTILE,
		EFFECT_END
	};

protected:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect(const CEffect& rhs);
	virtual ~CEffect(void);

public:
	virtual CEffect* Clone(void) PURE;

public:	// Setter
	virtual void Set_OriginPos(_vec3 OriginPos);
	void Set_EffectColor(_vec4 Color) { m_vColor = Color; }
	void Set_TimeToDead(_float fTime) {			// ���� �ð� ����
		m_bUseTimeDead = TRUE;
		m_fTime = fTime; }
	void Set_FadeOut(_float fFadeOutSpeed) { m_bFadeOut = TRUE; m_fFadeOutSpeed = fFadeOutSpeed; }
	void Set_Information(Engine::CTransform::INFORMATION eInfo, const _vec3* pVector);
	void Set_BillState(BILL_STATE State) { m_eBillState = State; }		// For. Single, Frame
	void Set_DetailPos(_vec3 DetailPos) { m_vDetailPos = DetailPos; }		// ���� �ø��ų� ���϶�
	void Set_StartTimeControl(_float fStartTime) { m_bIsStartTimeControl = TRUE; m_fStartTime = fStartTime; }
	void Set_ShaderPass(SHADER_EFFECT_PASS ePass) { m_eShaderPass = ePass; }
	void Set_InstantFadeOut(_float fFadeOutSpeed) { m_bInstantFadeOut = TRUE; m_fFadeOutSpeed = fFadeOutSpeed; }
	void Set_UseDotProduct(void) { m_bUseDotProduct = TRUE; }// ������ ����ؼ� ������ ��� �����ٰ��� . -> ��� �޾ƿ��� ��츸 ����.

public:	// Setter_Pos&Matrix
	void Set_FixMatrix(_matrix* matFix, _matrix* matParent) { m_pmatFix = matFix; m_pmatParent = matParent; }
	void Set_FixMatrix(_matrix* matFix) { m_pmatFix = matFix; }
	void Set_FinalPos(_vec3 FinalPos) { m_bUseFinalPos = TRUE; m_FinalPos = FinalPos; }
	void Set_UseDir(_vec3 Dir) { m_vEffectDir = Dir; m_bUseDir = TRUE; }
	void Set_EffectSpeed(_float fSpeed) { m_fSpeed = fSpeed; }
	void Set_TargetPos(_vec3 vTargetPos) { m_vTargetPos = vTargetPos; }

public:	// Getter
	_vec3 Get_Information(Engine::CTransform::INFORMATION eInfo);

public:
	virtual HRESULT Ready_GameObject(void) { return NOERROR; }
	virtual int Update_GameObject(const float & fTimeDelta) { return 0; }
	virtual void Render_GameObject(void) {}
	virtual void Reset_Effect(void) {}

protected:
	void Bill_Perform(BILL_STATE BillState);	// For. Single, Frame
	void Revision_Pos(void);
	void Toward_FinalPos(const float & fTimeDelta);

	// �����ٺ��ϱ� �������� bool������ float , Set���� �þ�� ��������
	// ���� �ʿ��ҵ� ?

protected:		// ���������� ����ϴ� Bool�� State
	_bool			m_bUseTimeDead;		// �ð��� ���� ���������
	_bool			m_bFadeIn;					// �����Ҷ� FadeIn
	_bool			m_bFadeOut;				// ������� FadeOut
	_bool			m_bInstantFadeOut;		// �̰� True�� , �ٷ� fadeOut->�����ȴ�.
	_float			m_fFadeOutSpeed;

protected:
	_float			m_fTime;			// ����Ʈ�� ����ִ� �ð�
	_vec4			m_vColor;			// ���̴��� ���� ����
	_vec3			m_vDetailPos;		// �������ϰ� Pos �����Ҷ� ��� -> Default (0.0.0)

protected:		// ����Ʈ�� Ȱ���� �����ϴ� �ð� ���� (?
	_bool			m_bIsStartTimeControl;
	_float			m_fStartTime;

protected:	// ����Ʈ�� ��ġ ���� ����ϴ� ����
	_matrix*						m_pmatFix;
	_matrix*						m_pmatParent;

protected:	// FinalPos���ϴ� ����Ʈ�� ���
	_vec3							m_FinalPos;
	_float							m_fSpeed;
	_bool							m_bUseFinalPos;
	_bool							m_bUseDir;
	_vec3							m_vEffectDir;

protected:
	_bool							m_bUseDotProduct;		// ���� �̿�. (�̻��� ���� ..)
	_vec3							m_vTargetPos;				// ->Ÿ��Pos

protected:
	BILL_STATE					m_eBillState;
	SHADER_EFFECT_PASS	m_eShaderPass = EFFECT_END;
	CDataManager*			m_pDataMgr = nullptr;

protected:
	virtual void Free(void);
};

END

#endif // Effect_h__