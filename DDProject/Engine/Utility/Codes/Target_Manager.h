#pragma once

#include "Engine_Defines.h"

// Target_Manager :  여러개의  CTimer

BEGIN(Engine)

class CTarget;
class ENGINE_DLL CTarget_Manager : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	explicit CTarget_Manager();
	virtual ~CTarget_Manager();
public:
	HRESULT Add_RenderTarget(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTargetTag, const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color, bool bShadowMap = false);
	HRESULT SetUp_OnGraphicDev(const _tchar* pTargetTag, const _uint& iIndex);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const _tchar* pTargetTag, const char* pConstantName);
public:
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	HRESULT Begin_MRT(const _tchar* pMRTTag);
	HRESULT End_MRT(const _tchar* pMRTTag);
public:
	HRESULT Begin_SingleTarget(const _tchar* pTargetTag);
	HRESULT End_SingleTarget(const _tchar* pTargetTag);
public:
	HRESULT Ready_DebugBuffer(const _tchar* pTargetTag, _vec4* pBufferInfo);
	HRESULT Render_DebugBuffer(const _tchar* pTargetTag);
private:
	unordered_map<const _tchar*, CTarget*>			m_mapTargets;
	typedef unordered_map<const _tchar*, CTarget*>	MAPTARGETS;
private:
	unordered_map<const _tchar*, list<CTarget*>>	m_mapMRT;
	typedef unordered_map<const _tchar*, list<CTarget*>>	MAPMRT;
private:
	CTarget* Find_Target(const _tchar* pTargetTag);
	list<CTarget*>* Find_MRT(const _tchar* pMRTTag);
private:
	virtual void Free(void);

};

END

