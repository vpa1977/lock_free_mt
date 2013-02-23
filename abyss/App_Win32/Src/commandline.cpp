#include "../Include/commandline.h"

std::string CCommandLineParamerers::m_CommandLine("");
std::string CCommandLineParamerers::m_ExeName("");

const std::string&
CCommandLineParamerers::GetCommandLine()
{
	return m_CommandLine;
}

const std::string&
CCommandLineParamerers::GetExeName()
{
	return m_ExeName;
}





