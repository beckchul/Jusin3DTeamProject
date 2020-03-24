#pragma once
#ifndef Frame_Manager_h__
#define Frame_Manager_h__

#include "Frame.h"

BEGIN(Engine)
class ENGINE_DLL CFrame_Manager : public CBase
{
	DECLARE_SINGLETON(CFrame_Manager)
public:
	explicit CFrame_Manager(void);
	virtual ~CFrame_Manager(void);

public: // Getter	
public:
	HRESULT Ready_Frame(const TCHAR* pFrameTag, CFrame* pFrame);
	bool Permit_Call(const TCHAR* pFrameTag, const float& fTimeDelta);

private:
	unordered_map<const TCHAR*, CFrame*>			m_mapFrame;
	typedef unordered_map<const TCHAR*, CFrame*>	MAPFrame;

private:
	CFrame* Find_Frame(const TCHAR* pFrameTag) const;

private:
	virtual void Free(void) override;
};

END

#endif