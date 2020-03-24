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
	void Get_Position(void); // Ŭ���̾�Ʈ���� UI ��ġ ���
protected:
	virtual void Free(void);
};

