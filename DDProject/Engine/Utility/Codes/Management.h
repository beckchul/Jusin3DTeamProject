#ifndef Management_h__
#define Management_h__

#include "Engine_Defines.h"
#include "GameObject.h"

// ���� �������� ���� �����͸� ������ ������
// �� ���� ���� �Ǵ� �׸��� �۾��� �����Ѵ�.
// Renderer

BEGIN(Engine)

class CScene;
class CRenderer;
class CComponent;
class ENGINE_DLL CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)
private:
	explicit CManagement(void);
	virtual ~CManagement(void);	
public:
	HRESULT Ready_Management(const _uint& uIndex, const _tchar* pRendererTag);
	int Update_Management(const _float& fTimeDelta);
	void Render_Management(void);
	HRESULT SetUp_CurrentScene(CScene* pCurrentScene);
private:
	CScene*			m_pCurrentScene;
	CRenderer*		m_pRenderer;
protected:
	bool			m_bAfterFirstRender = false; // �� Scene�� ù ��° ������ ��������
private:
	virtual void Free(void);
};

END

#endif // Management_h__
