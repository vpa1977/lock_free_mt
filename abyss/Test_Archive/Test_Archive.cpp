// Test_Archive.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../VFS/Include/CZipFile.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc ==2 ) 
	{
		CZipFile f;
		f.Decompress(argv[1], ".");
	}
	if (argc!= 3 )  {
		printf("zar - abyss engine compression utility, based on zlib library\n");
		printf("Usage:\n");
		printf("zar <archive-file> <directory-to-compress>");

	}
	else
	{
		CZipFile f;
		f.Create(argv[2], argv[1]);
	}
	return 0;
}

