#include "Frame.h"

USING(Engine)

CFrame::CFrame()
	: m_fTimePerSec(0.f)
	, m_fTimeDeltaAcc(0.f)
{
}

CFrame::~CFrame()
{
}

HRESULT CFrame::Ready_Frame(const float & fCallCntPerSec)
{
	m_fTimePerSec = 1.f / fCallCntPerSec;

	return NOERROR;
}

// 내가 정한 시간이 지나면
bool CFrame::Permit_Call(const float& fTimeDelta)
{
	m_fTimeDeltaAcc += fTimeDelta;

	if(m_fTimePerSec <= m_fTimeDeltaAcc)
	{
		m_fTimeDeltaAcc = 0.f;
		return true;
	}

	return false;
}

CFrame * CFrame::Create(const float & fCallCntPerSec)
{
	CFrame *	pInstance = new CFrame();

	if (FAILED(pInstance->Ready_Frame(fCallCntPerSec)))
	{
		MSG_BOX(L"CFrame Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFrame::Free(void)
{
}
