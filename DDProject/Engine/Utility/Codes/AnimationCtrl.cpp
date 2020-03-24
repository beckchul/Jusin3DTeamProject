#include "AnimationCtrl.h"

USING(Engine)

Engine::CAnimationCtrl::CAnimationCtrl(LPDIRECT3DDEVICE9 pGraphicDev)
: m_pGraphicDev(pGraphicDev)
, m_pAnimationCtrl(NULL)
, m_dwCurrentTrack(0)
, m_dwNewTrack(1)
, m_dTimeAcc(0.0)
, m_iOldIndex(999)
, m_iMaxNumAniSet(0)
{
	m_pGraphicDev->AddRef();
}

Engine::CAnimationCtrl::CAnimationCtrl(const CAnimationCtrl& rhs)
: m_pGraphicDev(rhs.m_pGraphicDev)
, m_dwCurrentTrack(rhs.m_dwCurrentTrack)
, m_dwNewTrack(rhs.m_dwNewTrack)
, m_dTimeAcc(rhs.m_dTimeAcc)
, m_iOldIndex(rhs.m_iOldIndex)
, m_iMaxNumAniSet(rhs.m_iMaxNumAniSet)
{
	rhs.m_pAnimationCtrl->CloneAnimationController(
		rhs.m_pAnimationCtrl->GetMaxNumAnimationOutputs()
		, m_iMaxNumAniSet = rhs.m_pAnimationCtrl->GetMaxNumAnimationSets()
		, rhs.m_pAnimationCtrl->GetMaxNumTracks()
		, rhs.m_pAnimationCtrl->GetMaxNumEvents()
		, &m_pAnimationCtrl);	

	m_pGraphicDev->AddRef();
}

Engine::CAnimationCtrl::~CAnimationCtrl(void)
{

}

DOUBLE CAnimationCtrl::GetTrackPos(void)
{
	D3DXTRACK_DESC		TrackInfo;
	ZeroMemory(&TrackInfo, sizeof(D3DXTRACK_DESC));

	m_pAnimationCtrl->GetTrackDesc(m_dwCurrentTrack, &TrackInfo);

	return TrackInfo.Position;
}

HRESULT Engine::CAnimationCtrl::Ready_AnimationCtrl(void)
{
	return S_OK;
}

void Engine::CAnimationCtrl::Set_AnimationSet(ANI_INFO tAniInfo)
{
	if (m_iMaxNumAniSet <= tAniInfo.uiAniIndex)
		return;

	//if (m_iOldIndex == tAniInfo.uiAniIndex)
	//	return;


	LPD3DXANIMATIONSET	pAS = nullptr;

	m_pAnimationCtrl->GetAnimationSet(tAniInfo.uiAniIndex, &pAS);

	if (nullptr == pAS)
		return;

	m_dPeriod = pAS->GetPeriod();
	m_dwNewTrack = m_dwCurrentTrack == 0 ? 1 : 0;

	m_pAnimationCtrl->SetTrackAnimationSet(m_dwNewTrack, pAS);

	Engine::Safe_Release(pAS);

	m_pAnimationCtrl->UnkeyAllTrackEvents(m_dwCurrentTrack);
	m_pAnimationCtrl->UnkeyAllTrackEvents(m_dwNewTrack);

	m_pAnimationCtrl->KeyTrackEnable(m_dwCurrentTrack, FALSE, m_dTimeAcc + tAniInfo.dMagicNumber);
	m_pAnimationCtrl->KeyTrackWeight(m_dwCurrentTrack, tAniInfo.fAniWeightFront, m_dTimeAcc, tAniInfo.dMagicNumber, D3DXTRANSITION_LINEAR);
	m_pAnimationCtrl->KeyTrackSpeed(m_dwCurrentTrack, tAniInfo.fAniSpeedFront, m_dTimeAcc, tAniInfo.dMagicNumber, D3DXTRANSITION_LINEAR);

	m_pAnimationCtrl->SetTrackEnable(m_dwNewTrack, TRUE);
	m_pAnimationCtrl->KeyTrackWeight(m_dwNewTrack, tAniInfo.fAniWeightEnd, m_dTimeAcc, tAniInfo.dMagicNumber, D3DXTRANSITION_LINEAR);
	m_pAnimationCtrl->KeyTrackSpeed(m_dwNewTrack, tAniInfo.fAniSpeedEnd, m_dTimeAcc, tAniInfo.dMagicNumber, D3DXTRANSITION_LINEAR);

	m_pAnimationCtrl->ResetTime();
	m_dTimeAcc = 0.0;
	m_dwCurrentTrack = m_dwNewTrack;
	m_pAnimationCtrl->SetTrackPosition(m_dwNewTrack, 0.0);
	m_pAnimationCtrl->SetTrackPosition(m_dwCurrentTrack, 0.0);
}

void CAnimationCtrl::Set_TrackPosition(const _int& iIndex, const _float& fTimeDelta)
{
	m_pAnimationCtrl->SetTrackPosition(iIndex, fTimeDelta);
}

void Engine::CAnimationCtrl::Play_AnimationSet(const _float& fTimeDelta)
{
	m_pAnimationCtrl->AdvanceTime(fTimeDelta, NULL);
	m_dTimeAcc += fTimeDelta;
}


CAnimationCtrl* Engine::CAnimationCtrl::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAnimationCtrl*		pInstance = new CAnimationCtrl(pGraphicDev);

	if(FAILED(pInstance->Ready_AnimationCtrl()))
	{
		MSG_BOX(L"CAnimationCtrl Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimationCtrl* Engine::CAnimationCtrl::Create(const CAnimationCtrl& rhs)
{
	CAnimationCtrl*		pInstance = new CAnimationCtrl(rhs);

	return pInstance;
}

void Engine::CAnimationCtrl::Free(void)
{
	Engine::Safe_Release(m_pAnimationCtrl);
	Engine::Safe_Release(m_pGraphicDev);
}

