#include "Log.h"
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
	g_Handle = fopen("log.txt","wt");
	assert(g_Handle!=NULL);
}

void
CLog::Close()
{
	assert(g_Handle!=NULL);
	fclose(g_Handle);
	g_Handle=NULL;
}

void
CLog::Print(const char* in_pszFormat, ...)
{
	unsigned long T = timeGetTime();
	fprintf(g_Handle,"%lu: ",T);
	assert(in_pszFormat);
	va_list args;
	va_start(args, in_pszFormat);
	vfprintf(g_Handle,in_pszFormat,args);
}

void
CLog::Println(const char* in_pszFormat, ...)
{
	unsigned long T = timeGetTime();
	fprintf(g_Handle,"%lu: ",T);
	assert(in_pszFormat);
	va_list args;
	va_start(args, in_pszFormat);
	vfprintf(g_Handle,in_pszFormat,args);
	vfprintf(g_Handle,"\n");
}

