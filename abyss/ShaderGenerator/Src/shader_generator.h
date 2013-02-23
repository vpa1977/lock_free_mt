#ifndef SHADER_GENERATOR_HPP_INCLUDED
#define SHADER_GENERATOR_HPP_INCLUDED

#include "linker_interface.h"
#include <stdio.h>

// Runs shader creation sequence.
// And saves the results.
class CShaderGenerator
{
public:
	CShaderGenerator();
	~CShaderGenerator();
	bool Generate();
private:
	HMODULE m_hDll;
	INVLink* m_pLinker;

	void WriteHeader( FILE* f );
	void WriteFooter( FILE* f );
};

#endif


































