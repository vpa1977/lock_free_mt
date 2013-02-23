#ifndef RTASSERT_HPP_INCLUDED
#define RTASSERT_HPP_INCLUDED

void MyAssert( const char* in_Expression, const char* in_File, long in_Line );

#define RTASSERT(exp)	(void)( (exp) || ( MyAssert(#exp, __FILE__, __LINE__), 0 ) )

#endif

