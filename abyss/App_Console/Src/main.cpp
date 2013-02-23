#include "../../MT_Core/Include/thread.h"
#include "../../MT_Core/Include/log.h"

#include <stdio.h>

int
main
( int argc, char* argv [] )
{
	CLog::Init();
	{
		CLog::Print("main() begin -----------------------\n");
		{
			mp_manager::get_instance()->run_system();
		}
		CLog::Print("main() end -------------------------\n");
	}
	CLog::Close();
	return 0;
}

