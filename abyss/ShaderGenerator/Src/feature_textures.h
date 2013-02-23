#ifndef FEATURE_TEXTURES_HPP_INCLUDED
#define FEATURE_TEXTURES_HPP_INCLUDED

#include "abstract_feature.h"

class CFeature_Textures : public CAbstractFeature
{
public:
	CFeature_Textures();
	~CFeature_Textures();

	void AddFragments( INVLink* in_pLinker );
	void GetMyIDs( INVLink* in_pLinker );

	void Reset();

	// setup required stuff
	bool PreGenerate( CShaderResources& rRes, const CInputParams& rParams );

	// generate
	std::list<NVLINK_FRAGMENT_ID> GetFragmentsForThisCombination( const CShaderResources& rRes, const CInputParams& rParams );
	// post-generate
	void GetConstants( INVLink* in_pLinker );

	std::vector< std::pair<unsigned long,unsigned long> > GetBindings() const;

	//// false - next combination was set successfully
	//// true - this feature combination was reset. Need to increment next feature.
	//bool Next();

private:
	CShaderResources	m_Res;
	CInputParams		m_Params;

	NVLINK_FRAGMENT_ID	m_Fragment_Tex0_Passthru;
	NVLINK_FRAGMENT_ID	m_Fragment_Tex1_Passthru;
};


#endif




































