#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
	template <typename T> void Safe_Delete (T& pointer)
	{
		if(nullptr != pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}	

	template <typename T> void Safe_Delete_Array (T& pointer)
	{

		if(nullptr != pointer)
		{
			delete [] pointer;
			pointer = nullptr;
		}
	}


	template <typename TC> 
	unsigned long Safe_Release (TC& pointer)
	{
		unsigned long dwRefCnt = 0;

		if(nullptr != pointer)
		{
			dwRefCnt = pointer->Release();

			if(0 == dwRefCnt)
				pointer = nullptr;
		}

		return dwRefCnt;
	}	
}

#endif // Engine_Function_h__
