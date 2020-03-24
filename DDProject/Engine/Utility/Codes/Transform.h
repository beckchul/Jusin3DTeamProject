#ifndef Transform_h__
#define Transform_h__

#include "Engine_Defines.h"
#include "Component.h"

// 객체를 움직이고 그리기위한 데이터와 기능을 가진다.

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
public:
	enum ANGLE {ANGLE_X, ANGLE_Y, ANGLE_Z};
	enum INFORMATION {INFO_SCALE, INFO_ANGLE, INFO_POSITION, INFO_END};
private:
	explicit CTransform(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform(void);
public:
	// Getter
	_matrix* Get_WorldMatrix(void) {
		return &m_matWorld;}
	_vec3 Get_Info(INFORMATION eInfo) {
		return m_vInformation[eInfo];}
public:
	// Setter
	void Set_Information(INFORMATION eInfo, const _float& fX, const _float& fY, const _float& fZ );
	void Set_Information(INFORMATION eInfo, const _vec3* pVector, bool bUpdate = false);
	void Change_Information(INFORMATION eInfo, const _vec3* pVector);
	void Set_WorldMatrix(const _matrix* matWorld);
	void Change_WorldMatrix(const _vec3* pVector);
	void Set_ParentMatrix(_matrix* pParentMatrix);
	void SetUp_OnGraphicDev(void);
	void SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName); // 미리 행렬을 곱해서 전달하면 더 빠르다.
	void SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName, _matrix matView, _matrix matProj); // 미리 행렬을 곱해서 전달하면 더 빠르다.
	void SetRotation(ANGLE eType, const _float& fAngleTimeDelta);
public:
	HRESULT Ready_Component(void);
	virtual int Update_Component(void);
public:
	DWORD Go_Straight(const _float& fSpeedTimeDelta, DWORD dwIndx = 0, bool bDropoAble = true);
	DWORD Go_Right(const _float& fSpeedTimeDelta, DWORD dwIndx = 0, bool bDropoAble = true);
	void Go_FlyStraight(const _float& fSpeedTimeDelta);
	void Go_FlyRight(const _float& fSpeedTimeDelta);
	void Go_Down(const _float& fSpeedTimeDelta);
	void Rotation(ANGLE eType, const _float& fAngleTimeDelta);
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
private:	
	_vec3				m_vInformation[INFO_END];
	_matrix				m_matWorld;
	_matrix*			m_pParentMatrix;
public:
	virtual CComponent* Clone(void);
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

END

#endif // Transform_h__
