#include "shader_generator.h"

int
main( int argc, char* argv[] )
{
	CShaderGenerator sg;
	if( sg.Generate() ) {
		// success
		return 0;
	} else {
		// fail
		return -1;
	}
}





















