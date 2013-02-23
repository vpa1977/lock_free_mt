#ifndef RENDER_SECTION_INIT_RESULT_HPP_INCLUDED
#define RENDER_SECTION_INIT_RESULT_HPP_INCLUDED

enum INIT_RENDER_RESULT
{
	IRR_OK					= 0,
	IRR_ALREADY_INITIALIZED	= 1, // render is already initialized
	IRR_PARAMETERS			= 2, // wrong parameters
	IRR_FAIL				= 3
};

#endif