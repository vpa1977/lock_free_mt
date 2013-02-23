#include "../Include/Log.h"
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <windows.h>

#pragma comment( lib, "winmm.lib" )

FILE* g_Handle = NULL;

void
CLog::Init()
{
	assert(g_Handle==NULL);

	WIN32_FIND_DATA FindFileData;
	HANDLE HandleFind;
	HandleFind = FindFirstFile( "debug.txt", &FindFileData );
	if(HandleFind != INVALID_HANDLE_VALUE)
	{
		FindClose(HandleFind);
		g_Handle = fopen("log.txt","wt");
		assert(g_Handle!=NULL);
		Print("CLog::Init()\n");
	}
}

void
CLog::Close()
{
	if(g_Handle!=NULL)
	{
		Print("CLog::Close()\n");
		fclose(g_Handle);
		g_Handle=NULL;
	}
}

void
CLog::Print(const char* in_pszFormat, ...)
{
	if(g_Handle!=NULL)
	{
		unsigned long T = timeGetTime();
		fprintf(g_Handle,"%lu: ",T);
		assert(in_pszFormat);
		va_list args;
		va_start(args, in_pszFormat);
		vfprintf(g_Handle,in_pszFormat,args);
		fflush(g_Handle);
	}
}


void
CLog::Println(const char* in_pszFormat, ...)
{
	if(g_Handle!=NULL)
	{
		unsigned long T = timeGetTime();
		fprintf(g_Handle,"%lu: ",T);
		assert(in_pszFormat);
		va_list args;
		va_start(args, in_pszFormat);
		vfprintf(g_Handle,in_pszFormat,args);
		fprintf(g_Handle,"\n");
	}
}

void 
CLog::TraceEntry(const char* in_pszMethod)
{
	Println("Enter: %s",in_pszMethod);
}
void 
CLog::TraceExit(const char* in_pszMethod)
{
	CLog::Println("Exit: %s",in_pszMethod);
}


