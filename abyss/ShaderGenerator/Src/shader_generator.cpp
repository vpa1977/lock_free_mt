#include "shader_generator.h"
#include "shader.h"
#include <stdio.h>
#include <list>

CShaderGenerator::CShaderGenerator() : m_hDll(NULL), m_pLinker(NULL) {
	m_hDll = LoadLibrary("nvlinker.dll");
	assert( m_hDll != NULL );
	LPCREATELINKER pCreate = (LPCREATELINKER)GetProcAddress(m_hDll, "CreateLinker");
	assert( pCreate != NULL );
	m_pLinker = pCreate( 100, OPTIMIZERLEVEL_BASIC );
	//m_pLinker = pCreate( 100, OPTIMIZERLEVEL_NONE );
	assert( m_pLinker );
}

CShaderGenerator::~CShaderGenerator() {
}

bool
CShaderGenerator::Generate() {
	printf("Generating.......\n");

	FILE* f = fopen("shader_definitions.inl","wt");
	WriteHeader(f);

	CShader s(m_pLinker);
	s.Reset();
	bool Ok = false;
	std::list< std::string > Structs;
	for( ; ; ) {
		printf("=======================================================\n");
		if( !s.Generate( m_pLinker ) )
			break;

		{
			// save result for this step
			std::string Ident = s.GetMeaningfulIdentifier();
			//if( Ident == "XYZ_LightDisabled_11") {
			//	int i = 666;
			//}

			fprintf(f,"unsigned long %s_BINARY[] = {",Ident.c_str());
			std::vector< unsigned long > Bin = s.GetBinary();
			for( size_t i=0; i<Bin.size(); ++i ) {
				fprintf(f,"%s 0x%08X", i==0?"":",", Bin[i] );
			}
			fprintf(f," };\n");

			fprintf(f,"unsigned long %s_DECLARATION[] = {",Ident.c_str());
			std::vector< DWORD > Decl = s.GetDeclaration();
			for( size_t i=0; i<Decl.size(); ++i ) {
				fprintf(f,"%s 0x%08X", i==0?"":",", Decl[i] );
			}
			fprintf(f," };\n");

			fprintf(f,"unsigned long %s_CONSTBINDINGS[] = {",Ident.c_str());
			std::vector< std::pair<unsigned long,unsigned long> > Bind = s.GetConstantBindings();
			for( size_t i=0; i<Bind.size(); ++i ) {
				fprintf(f,"%s 0x%08X", i==0?"":",", Bind[i].first );
				fprintf(f,", 0x%08X", Bind[i].second );
			}
			fprintf(f," };\n");

			char Buffer[1024];
			sprintf(Buffer,"{ 0x%08X, 0x%08X, 0x%08X, %s_BINARY, %s_DECLARATION, %s_CONSTBINDINGS }", s.GetFVF(),s.GetLights(),s.GetTextures(),Ident.c_str(),Ident.c_str(),Ident.c_str() );
			Structs.push_back( std::string(Buffer) );
		}

		if( !s.NextCombination() ) {
			Ok = true;
			break;
		}
	}

	if(Ok) {
		fprintf(f,"\nCShaderDescriptionGenerated g_GeneratedShaders[] = {\n");
		for( std::list< std::string >::const_iterator It=Structs.begin(); It!=Structs.end(); ++It ) {
			fprintf(f,"  %s%s\n", It==Structs.begin() ? "":",", It->c_str());
		}
		WriteFooter(f);
	}

	fclose(f);
	return Ok;
}

void
CShaderGenerator::WriteHeader( FILE* f ) {
	fprintf(f,"struct CShaderDescriptionGenerated {\n  unsigned long m_FVF;\n  unsigned long m_Lights;\n  unsigned long m_Textures;\n  unsigned long* m_Binary;\n  unsigned long* m_Declaration;\n  unsigned long* m_ConstantBindings;\n};\n\n");
}

void
CShaderGenerator::WriteFooter( FILE* f ) {
	fprintf(f,"};\n");
	fprintf(f,"#define N_GENERATED_SHADERS (sizeof(g_GeneratedShaders)/sizeof(CShaderDescriptionGenerated))\n");
}

/*
struct CShaderDescriptionGenerated {
	unsigned long m_FVF;
	unsigned long m_Lights;
	unsigned long m_Textures;
	unsigned long* m_Binary;
	unsigned long* m_Declaration;
	unsigned long* m_ConstantBindings;
};
*/


































