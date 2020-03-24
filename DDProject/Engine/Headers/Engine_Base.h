#ifndef Engine_Base_h__
#define Engine_Base_h__

class _declspec(dllexport) CBase
{
protected:
	 explicit CBase(void) : m_dwRefCnt(0) {}
	 virtual ~CBase(void) {}
public: // �Լ��̸� : ����_���
	// RefCnt������Ų��.
	 unsigned long Add_Ref(void) { return ++m_dwRefCnt; }

	// RefCnt���ҽ�Ų��. ��, RefCnt 0�̶�� �����Ѵ�.
	unsigned long Release(void)
	{
		if (0 == m_dwRefCnt)
		{
			Free();
			delete this;

			return 0;
		}
		else
			return m_dwRefCnt--;
	}

protected:
	// �ش�Ŭ������ �Ҵ�� ������ �ּҸ� �����ϰ��ִ� �������������� ����.
	unsigned long		m_dwRefCnt = 0;
protected:
	virtual void Free(void) = 0;
};

#endif // Base_h__