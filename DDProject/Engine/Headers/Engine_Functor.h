
#ifndef Engine_Functor_h__
#define Engine_Functor_h__

namespace Engine
{
	class CRelease_Pair
	{
	public:
		explicit CRelease_Pair(void){}
		~CRelease_Pair(void){}
	public: // operator	
		template <typename TC> 
		unsigned long operator () (TC& Pair)
		{			
			DWORD		dwRefCnt = 0;
			if(nullptr != Pair.second)
			{
				dwRefCnt = Pair.second->Release();
				if (0 == dwRefCnt)
					Pair.second = nullptr;				
			}
			return dwRefCnt;
		}
	};

	class CRelease_Single
	{
	public:
		explicit CRelease_Single(void) {}
		~CRelease_Single(void) {}
	public: // operator
		template <typename TC> unsigned long operator () (TC& pInstance)
		{		
			DWORD		dwRefCnt = 0;

			if(nullptr != pInstance)
			{
				dwRefCnt = pInstance->Release();
				if(0 == dwRefCnt)
					pInstance = nullptr;
			}
			return dwRefCnt;
		}
	};


	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const TCHAR* pTag) : m_pTag(pTag) {}
		~CTag_Finder(void) {}
	public:
		template <typename TC> bool operator () (TC& Pair)
		{
			int iResult = lstrcmp(m_pTag, Pair.first);

			if(0 == iResult)
				return true;
			return false;
		}
	private:
		const TCHAR*				m_pTag;
	};
}

#endif // Engine_Functor_h__