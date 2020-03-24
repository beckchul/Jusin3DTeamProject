#ifndef Engine_Base_h__
#define Engine_Base_h__

class _declspec(dllexport) CBase
{
protected:
	 explicit CBase(void) : m_dwRefCnt(0) {}
	 virtual ~CBase(void) {}
public: // 함수이름 : 동사_명사
	// RefCnt증가시킨다.
	 unsigned long Add_Ref(void) { return ++m_dwRefCnt; }

	// RefCnt감소시킨다. 단, RefCnt 0이라면 삭제한다.
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
	// 해당클래스로 할당된 공간의 주소를 참조하고있는 포인터형변수의 갯수.
	unsigned long		m_dwRefCnt = 0;
protected:
	virtual void Free(void) = 0;
};

#endif // Base_h__