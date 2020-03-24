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

	virtual void Change_Camera(_uint uNum = 0, float fAttachTime = 0.f) PURE; // 본인 카메라로 바뀌었을 때
	
	void Initialize_Camera(void); // 카메라의 위치와 방향을 현재 matView의 정보로 초기화

	// 자연스러운 카메라 전환
	void Flow_Camera(const _float& fTimeDelta, _vec3& vecDest, _float fDistance, _vec3& vecDir, _float fAngle, _vec3& vecAxis, _float fTime);

protected:
	// View Matrix
	_vec3				m_vEye, m_vAt, m_vUp;
	_vec3				m_vDir;
	_matrix				m_matView;

	// Proj Matrix
	_float				m_fFovy, m_fAspect, m_fNear, m_fFar;
	_matrix				m_matProj;

	// 카메라의 자연스러운 이동과 회전
	_vec3				m_vDestEye; // 목표 위치
	_float				m_fDistance = 0.f; // 목표 거리

	_vec3				m_vDestDir; // 목표 방향
	_float				m_fAngle = 0.f; // 목표 각
	_vec3				m_vAxis; // 회전 축

	_bool				m_bCamAttach = true; 
	_float				m_fAttachTime = 0.f; // 카메라가 제자리를 찾는게 걸리는 시간
	_float				m_fTime = 0.f;

protected:
	CDataManager*		m_pDataMgr = nullptr;
protected:
	virtual void Free(void);
};

END

#endif // Camera_h__
