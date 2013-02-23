/************************************************************************
*
* Testing CmemoryPoolGranulated
*
*************************************************************************/

#include "../MT_Core/Include/memory_pool.h"
#include <deque>
#include <stdio.h>
#include <conio.h>
#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>
#include <malloc.h>
#include <windows.h>
#include <crtdbg.h>

typedef unsigned char BYTE;

void
GenerateJunk(void* in_pMem, size_t in_Size)
{
	BYTE* pPtr = (BYTE*)in_pMem;
	BYTE* pEnd = pPtr+in_Size;
	while(pPtr<pEnd)
	{
		//*pPtr = (rand()%0xff);
		*pPtr = 0x55;
		pPtr++;
	}
}

class CMemPatterns
{
public:
	/*
	~CMemPatterns()
	{
		assert(m_Sizes.empty());
		assert(m_Pointers.empty());
	}
	*/
	void* Generate(size_t& out_Size)
	{
		// [1...256]
		out_Size = 16*(1 + (rand()%256));
		/*
		void* pMem = malloc(out_Size);
		assert(pMem);
		GenerateJunk(pMem,out_Size);
		m_Sizes.push_back(out_Size);
		m_Pointers.push_back(pMem);
		return pMem;
		*/
		return NULL;
	}
	/*
	void* Peek(size_t& out_Size)
	{
		assert(!m_Sizes.empty());
		assert(!m_Pointers.empty());
		out_Size = m_Sizes.front();
		return m_Pointers.front();
	}
	*/
	/*
	void Release()
	{
		assert(!m_Sizes.empty());
		assert(!m_Pointers.empty());
		m_Sizes.pop_front();
		void* pMem = m_Pointers.front();
		assert(pMem);
		free(pMem);
		m_Pointers.pop_front();
	}
	*/
private:
	/*
	std::deque<size_t> m_Sizes;
	std::deque<void*> m_Pointers;
	*/
};

int
main( int argc, char* argv [] )
{

#ifdef _DEBUG
	int tmpDbgFlag;

	HANDLE hLogFile=CreateFile("log.txt",GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	
	_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE|_CRTDBG_MODE_WNDW|_CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_WARN,_CRTDBG_MODE_FILE|_CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE|_CRTDBG_MODE_WNDW|_CRTDBG_MODE_DEBUG);

	_CrtSetReportFile(_CRT_ASSERT,hLogFile);
	_CrtSetReportFile(_CRT_WARN,hLogFile);
	_CrtSetReportFile(_CRT_ERROR,hLogFile);

	tmpDbgFlag=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag|=_CRTDBG_ALLOC_MEM_DF;
	//tmpDbgFlag|=_CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag|=_CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
#endif


//CMemPatterns Patterns;
//CMemoryPoolGranulated Pool;
//std::deque<char> qq;

	printf("main() begin\n");
	char Buffer[1024];
	gets(Buffer);
	size_t MemoryAllocated = 0;
	long MaxChunksAllocated = 0;
	size_t MaxMemoryAllocated = 0;
	{
		CMemPatterns Patterns;
		CMemoryPoolGranulated Pool;
		unsigned long Counter=0;
		while( !_kbhit() && Counter<0xffffffff )
		{
			Counter++;
			if(0==(Counter%1000))
				printf("%lu\n",Counter);
			int R = rand()%2;
			if(R>0 && Pool.GetSize()<4096)
			{
				// generate
				size_t Size=0;
				void* pGenerated = Patterns.Generate(Size);
				//assert(pGenerated);
				void* pAllocatedInPool = Pool.Allocate(Size);
				assert(pAllocatedInPool);
				//memcpy(pAllocatedInPool,pGenerated,Size);
				//
				MemoryAllocated += Size;
				if(Pool.GetSize()>MaxChunksAllocated) MaxChunksAllocated = Pool.GetSize();
				if(MemoryAllocated>MaxMemoryAllocated) MaxMemoryAllocated = MemoryAllocated;
			}
			else if(Pool.GetSize()>0)
			{
				// release
				//
				//size_t SizeRemembered = 0;
				//const void* pMemRemembered = Patterns.Peek(SizeRemembered);
				//assert(pMemRemembered);
				//
				size_t SizeAllocated = 0;
				const void* pMemAllocated = Pool.Peek(SizeAllocated);
				assert(pMemAllocated);
				//
				//assert(SizeRemembered==SizeAllocated);
				//int R = memcmp(pMemRemembered,pMemAllocated,SizeRemembered);
				//assert(0==R);
				Pool.Release(pMemAllocated);
				//Patterns.Release();
				// stats
				assert(MemoryAllocated>=SizeAllocated);
				MemoryAllocated -= SizeAllocated;
			}
		}
		// cleanup
		while(Pool.GetSize()>0)
		{
			//
			//size_t SizeRemembered = 0;
			//const void* pMemRemembered = Patterns.Peek(SizeRemembered);
			//assert(pMemRemembered);
			//
			size_t SizeAllocated = 0;
			const void* pMemAllocated = Pool.Peek(SizeAllocated);
			assert(pMemAllocated);
			//
			//assert(SizeRemembered==SizeAllocated);
			//int R = memcmp(pMemRemembered,pMemAllocated,SizeRemembered);
			//assert(0==R);
			Pool.Release(pMemAllocated);
			//Patterns.Release();
		}
	}
	printf("  MaxChunksAllocated = %ld\n",MaxChunksAllocated);
	printf("  MaxMemoryAllocated = %ld bytes\n",MaxMemoryAllocated);
	printf("main() end\n");
	while(_kbhit())
		_getch();
	gets(Buffer);

	return 0;
}



















