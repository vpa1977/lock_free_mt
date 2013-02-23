#ifndef LOG_HPP_INCLUDED
#define LOG_HPP_INCLUDED

class CLog
{
public:
	static void Init();
    static void Close();
	static void Print(const char* in_pszFormat, ...);
	static void Println(const char* in_pszFormat, ...);
	static void TraceEntry(const char* in_pszMethod);
	static void TraceExit(const char* in_pszMethod);
};

#endif
