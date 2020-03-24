#ifndef Camera_h__
#define Camera_h__

#include "GameObject.h"

BEGIN(Engine)

class CDataManager;
class ENGINE_DLL CCamera : public CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera(void);
public:
	// Setter
	void			Set_CamAttach(bool b) { m_bCamAttach = b; }

	// Getter
	_matrix*		Get_ViewMatrix(void) { return &m_matView; }
	_matrix*		Get_ProjMatrix(void) { return &m_matProj; }
	bool			Get_CamAttach(void) { return m_bCamAttach; }
public:
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);

	virtual void Change_Camera(_uint uNum = 0, float fAttachTime = 0.f) PURE; // ���� ī�޶�� �ٲ���� ��
	
	void Initialize_Camera(void); // ī�޶��� ��ġ�� ������ ���� matView�� ������ �ʱ�ȭ

	// �ڿ������� ī�޶� ��ȯ
	void Flow_Camera(const _float& fTimeDelta, _vec3& vecDest, _float fDistance, _vec3& vecDir, _float fAngle, _vec3& vecAxis, _float fTime);

protected:
	// View Matrix
	_vec3				m_vEye, m_vAt, m_vUp;
	_vec3				m_vDir;
	_matrix				m_matView;

	// Proj Matrix
	_float				m_fFovy, m_fAspect, m_fNear, m_fFar;
	_matrix				m_matProj;

	// ī�޶��� �ڿ������� �̵��� ȸ��
	_vec3				m_vDestEye; // ��ǥ ��ġ
	_float				m_fDistance = 0.f; // ��ǥ �Ÿ�

	_vec3				m_vDestDir; // ��ǥ ����
	_float				m_fAngle = 0.f; // ��ǥ ��
	_vec3				m_vAxis; // ȸ�� ��

	_bool				m_bCamAttach = true; 
	_float				m_fAttachTime = 0.f; // ī�޶� ���ڸ��� ã�°� �ɸ��� �ð�
	_float				m_fTime = 0.f;

protected:
	CDataManager*		m_pDataMgr = nullptr;
protected:
	virtual void Free(void);
};

END

#endif // Camera_h__
