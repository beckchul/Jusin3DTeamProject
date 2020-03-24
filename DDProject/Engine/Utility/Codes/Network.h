#ifndef Network_h__
#define Network_h__

#include <winsock2.h>
#include <WS2tcpip.h>
#include "Engine_defines.h"

BEGIN(Engine)

class CLayer;
class ENGINE_DLL CNetwork : public CBase
{
public:
	explicit CNetwork();
	virtual ~CNetwork();

public:
	HRESULT InitNetwork(LPDIRECT3DDEVICE9 pGraphicDev);

	void SendMsg(NETDATA tData);
	NETDATA RecvMsg(void);
public:
	virtual void RecvData(void)PURE;

	// ���� �÷��̾� ���Ӽ��� �˷��ִ� �Լ�
	int GetPlayerNum(void) { return m_iPlayerNum; }

protected:
	// ���� ����
	SOCKET				socket_client;
	int					m_iPlayerNum = 0;
	LPDIRECT3DDEVICE9   m_pGraphicDev = nullptr;

protected:
	virtual void Free(void);
};

END

#endif // !Network_h__