#include "../Include/rtassert.h"
#include <stdio.h>
#include <windows.h>

void
MyAssert( const char* in_Expression, const char* in_File, long in_Line )
{
	char Buffer[4096];
	sprintf(Buffer,"File %s,\nLine %ld\nExpression: %s",in_File,in_Line,in_Expression);
	MessageBox( NULL, Buffer, "Assertion failed !", MB_OK );
	abort();
}
















