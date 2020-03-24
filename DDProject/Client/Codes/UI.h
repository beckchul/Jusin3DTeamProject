#pragma once

#include "Defines.h"
#include "GameObject.h"

class CUI abstract : public Engine::CGameObject
{
public:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI();

protected:
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CRect_Texture*		m_pBufferCom = nullptr;
protected:
	vector<int> Compute_Digits(int iNum);
protected:
	void Get_Position(void); // 클라이언트에서 UI 위치 잡기
protected:
	virtual void Free(void);
};

