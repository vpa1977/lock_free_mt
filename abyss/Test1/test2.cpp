/*********************************************
*
*  Testing section registration
*
**********************************************/
#include "../MT_Core/Include/section.h"
#include "../MT_Core/Include/section_registrator.h"
#include "../MT_Core/Include/thread.h"
#include "../MT_Core/Include/idgen.h"
#include <stdio.h>

#include <windows.h>
#include <crtdbg.h>

int
main(int argc, char* argv[])
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

	printf("main() begin -----------------------\n");
	DumpRegisteredSections();
	{
		CThread::RunPool();
	}
	printf("main() end -------------------------\n");
	return 0;
}










