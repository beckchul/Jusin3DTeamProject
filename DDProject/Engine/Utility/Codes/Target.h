#pragma once

#include "Engine_Defines.h"

// 1. Ÿ���� �׸��ڸʿ����� �ƴ��� �˾���Ѵ�.

BEGIN(Engine)

class CView_Texture;
class ENGINE_DLL CTarget : public CBase
{
private:
	explicit CTarget(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTarget();
public:
	HRESULT Ready_Target(const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color, bool bShadowMap = false);
	HRESULT SetUp_OnGraphicDev(const _uint& iIndex);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName);
	HRESULT Release_OnGraphicDev(void);
	HRESULT Clear_Target(void);

public:
	HRESULT Ready_DebugBuffer(_vec4* pBufferInfo);
	HRESULT Render_DebugBuffer(void);

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
	LPDIRECT3DTEXTURE9			m_pTargetTexture = nullptr;
	LPDIRECT3DSURFACE9			m_pOldSurface = nullptr;
	LPDIRECT3DSURFACE9			m_pRenderTarget = nullptr; // ����Ÿ������ ����Ҽ� �ִ� ��ü.

	LPDIRECT3DSURFACE9			m_pShadowDepthStencil = nullptr; // �׸��ڸʿ����� ����� ���̹���
	LPDIRECT3DSURFACE9			m_pBackBuff = nullptr; // �������� �����
	LPDIRECT3DSURFACE9			m_pDepthStencilBuffer = nullptr; //�������� ���̹���

private:
	CView_Texture*				m_pBufferCom = nullptr;
private:
	_uint			m_iIndex = 0;
	D3DXCOLOR		m_Color = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
	bool			m_bShadowMap = false;
public:
	static CTarget* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color, bool bShadowMap = false);
private:
	virtual void Free(void);
};

END

