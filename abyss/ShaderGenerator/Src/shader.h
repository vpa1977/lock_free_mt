#ifndef SHADER_HPP_INCLUDED
#define SHADER_HPP_INCLUDED

#include "abstract_feature.h"
#include "input_params.h"
#include <vector>

#include "linker_interface.h"

// Creates shader test/code/declaration
// for different combinations of parameters.
class CShader
{
public:
	CShader( INVLink* in_pLinker );
	~CShader();
	// s.Reset();
	// do {
	//   s.Generate();
	// } while( s.NextCombination() );
	void Reset();
	bool NextCombination();
	bool Generate( INVLink* in_pLinker );

	// TODO
	std::string GetMeaningfulIdentifier() const { return m_Identifier; }
	unsigned long GetFVF() const { return m_FVF; }
	unsigned long GetLights() const { return m_Lights; }
	unsigned long GetTextures() const { return m_Textures; }
	// 2. additional params like amount/type of light sources, and matrix generation types
	std::vector< unsigned long > GetBinary() const { return m_Binary; }
	std::vector< DWORD > GetDeclaration() const { return m_Declaration; }
	std::vector< std::pair<unsigned long,unsigned long> > GetConstantBindings() const { return m_ConstantBindings; }
	// 6. shader text (?)

private:
	std::vector< CAbstractFeature* > m_Features;

	std::string m_Identifier;
	static std::string ConstructMeaningfulIdentifier( const CInputParams& );

	unsigned long m_FVF;
	static unsigned long ConstructFVF( const CInputParams& );

	unsigned long m_Lights;
	static unsigned long ConstructLights( const CInputParams& );

	unsigned long m_Textures;
	static unsigned long ConstructTextures( const CInputParams& );

	std::vector<unsigned long> m_Binary;

	std::vector<DWORD> m_Declaration;
	static std::vector<DWORD> ConstructDeclaration( const CInputParams& );

	std::vector< std::pair<unsigned long,unsigned long> > m_ConstantBindings;

	std::set<CInputParams> m_Processed;
	CInputParams m_Params;
	CInputParams m_GenerateParams;
	long m_ParamSeqNumber;
	void Dump( const CInputParams& in_P );
};

#endif


































