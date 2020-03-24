#pragma once
#ifndef Frame_h__
#define Frame_h__

#include "Engine_Defines.h"

BEGIN(Engine)

// ���� ���� �ʸ�ŭ ������ ��, true�� �����ϴ� �Լ��� ������.
class ENGINE_DLL CFrame : public CBase
{
private:
	explicit CFrame(void);
	virtual ~CFrame(void);

public:
	HRESULT Ready_Frame(const float& fCallCntPerSec);
	bool Permit_Call(const float& fTimeDelta);

private:
	float			m_fTimePerSec = 0.f;
	float			m_fTimeDeltaAcc = 0.f;

public:
	static CFrame* Create(const float& fCallCntPerSec);

private:
	virtual void Free(void) override;
};

END

#endif