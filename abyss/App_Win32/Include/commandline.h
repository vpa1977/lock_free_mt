#ifndef COMMANDLINE_HPP_INCLUDED
#define COMMANDLINE_HPP_INCLUDED

#include <string>
#include <windows.h>

class CCommandLineParamerers
{
public:
	static const std::string& GetCommandLine();
	static const std::string& GetExeName();
private:
	static std::string m_CommandLine;
	static std::string m_ExeName;

	friend int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
};

#endif



