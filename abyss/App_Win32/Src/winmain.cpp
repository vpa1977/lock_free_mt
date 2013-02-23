#include "../../MT_Core/Include/thread.h"
#include "../../MT_Core/Include/Log.h"
#include "../Include/commandline.h"
#include <windows.h>
#include <string>

extern void DumpRegisteredSections();

HINSTANCE	g_hInstance = NULL;
int			g_CmdShow = 0;
const char* g_CmdLine = NULL;

int APIENTRY
WinMain
(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CLog::Init();
	g_hInstance = hInstance;
	g_CmdLine = lpCmdLine;
	g_CmdShow = nCmdShow;
	CoInitialize(NULL);
	CCommandLineParamerers::m_CommandLine = lpCmdLine;
	{
		//DumpRegisteredSections();
		mp_manager::get_instance()->dump_sections();
		mp_manager::get_instance()->run_system();
		
	}
	CoUninitialize();
	CLog::Close();
	return 0;
}


