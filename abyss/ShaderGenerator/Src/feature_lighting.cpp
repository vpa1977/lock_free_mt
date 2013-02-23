#include "feature_lighting.h"
#include "../../Render_DX8/Include/r8_vs_tokens.h"

CFeature_Lighting::CFeature_Lighting() {
}

CFeature_Lighting::~CFeature_Lighting() {
}

void
CFeature_Lighting::AddFragments( INVLink* in_pLinker ) {
	//printf("CFeature_Lighting::AddFragments()\n");
	AddFragmentsInternal(in_pLinker,"lighting_disabled.nvo");
	AddFragmentsInternal(in_pLinker,"lighting_light0.nvo");
	AddFragmentsInternal(in_pLinker,"lighting_light1.nvo");
	AddFragmentsInternal(in_pLinker,"lighting_light2.nvo");
	AddFragmentsInternal(in_pLinker,"lighting_light3.nvo");
	AddFragmentsInternal(in_pLinker,"lighting_light_output.nvo");
}

void
CFeature_Lighting::GetMyIDs( INVLink* in_pLinker ) {
	//printf("CFeature_Lighting::GetMyIDs()\n");
	assert( m_Fragments.empty() );
	assert( m_ActiveFragments.empty() );

	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_LightDisabled() ) );

	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(0,CInputParams::LT_NONE) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(0,CInputParams::LT_DIRECTIONAL) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(0,CInputParams::LT_POINT) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(0,CInputParams::LT_SPOT) ) );

	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(1,CInputParams::LT_NONE) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(1,CInputParams::LT_DIRECTIONAL) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(1,CInputParams::LT_POINT) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(1,CInputParams::LT_SPOT) ) );

	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(2,CInputParams::LT_NONE) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(2,CInputParams::LT_DIRECTIONAL) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(2,CInputParams::LT_POINT) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(2,CInputParams::LT_SPOT) ) );

	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(3,CInputParams::LT_NONE) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(3,CInputParams::LT_DIRECTIONAL) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(3,CInputParams::LT_POINT) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Light(3,CInputParams::LT_SPOT) ) );

	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Output(true) ) );
	m_Fragments.push_back( boost::shared_ptr<CActiveFragment>( new CActiveFragment_Output(false) ) );

	for( std::vector< boost::shared_ptr<CActiveFragment> >::const_iterator It=m_Fragments.begin(); It!=m_Fragments.end(); ++It ) {
		(*It)->GetIDs(in_pLinker);
	}
	//HRESULT Res;
	//Res = in_pLinker->GetFragmentID("f_lighting_disabled", &m_FragmentID_Disabled );
	//assert( Res == S_OK );
	//Res = in_pLinker->GetConstantID("c_one_zero", &m_ConstantID_WhiteColor);
	//assert( Res == S_OK );
}

void
CFeature_Lighting::Reset() {
}

bool
CFeature_Lighting::PreGenerate( CShaderResources& rRes, const CInputParams& rParams ) {
	if(rParams.m_LightEnabled) {
		assert( rParams.m_Normal );
		if(rParams.m_Lights[0] != CInputParams::LT_NONE ) {
			// at least one light is active
			rRes.m_Required_NObject = true;
		}
	}
	//if(rParams.m_LightEnabled)
	//rRes.m_Required_NObject = true;
	return true;
}

std::list<NVLINK_FRAGMENT_ID>
CFeature_Lighting::GetFragmentsForThisCombination( const CShaderResources& rRes, const CInputParams& rParams ) {
	m_Res = rRes;
	m_Params = rParams;

	std::list<NVLINK_FRAGMENT_ID> Frags;

	m_ActiveFragments.clear();
	for( std::vector< boost::shared_ptr<CActiveFragment> >::const_iterator It=m_Fragments.begin(); It!=m_Fragments.end(); ++It ) {
		if( (*It)->BelongsToCurrentSetup(rRes,rParams) ) {
			m_ActiveFragments.push_back( *It );
		}
	}
	for( std::vector< boost::shared_ptr<CActiveFragment> >::const_iterator It=m_ActiveFragments.begin(); It!=m_ActiveFragments.end(); ++It ) {
		Frags.push_back( (*It)->GetID() );
	}

	return Frags;
}

void
CFeature_Lighting::GetConstants( INVLink* in_pLinker ) {
	for( std::vector< boost::shared_ptr<CActiveFragment> >::const_iterator It=m_ActiveFragments.begin(); It!=m_ActiveFragments.end(); ++It ) {
		(*It)->GetConstants(in_pLinker);
	}
}

std::vector< std::pair<unsigned long,unsigned long> >
CFeature_Lighting::GetBindings() const {
	//printf("  CFeature_Lighting::GetBindings()\n");
	std::map<unsigned long,unsigned long> Bindings;
	for( std::vector< boost::shared_ptr<CActiveFragment> >::const_iterator It=m_ActiveFragments.begin(); It!=m_ActiveFragments.end(); ++It ) {
		//printf("    active fragment: '%s'\n",(*It)->GetName().c_str());
		std::map<unsigned long,unsigned long> ThisFragmentBindings = (*It)->GetBindings();
		for(std::map<unsigned long,unsigned long>::const_iterator QQ=ThisFragmentBindings.begin(); QQ!=ThisFragmentBindings.end(); ++QQ ) {
			unsigned long TOKEN = QQ->first;
			assert( TOKEN <= 21 ); // max int value of Token
			unsigned long VALUE = QQ->second;
			assert( VALUE <= 127 ); // max const register
			//printf("      t=%d v=%d\n",TOKEN,VALUE);
			std::pair< std::map<unsigned long,unsigned long>::iterator, bool > Result = Bindings.insert( std::map<unsigned long,unsigned long>::value_type(TOKEN,VALUE) );
			if(Result.second == false) {
				std::map<unsigned long,unsigned long>::iterator WhereFound = Result.first;
				unsigned long PREV_VALUE = WhereFound->second;
				assert( PREV_VALUE == VALUE );
			}
		}
	}
	std::vector< std::pair<unsigned long,unsigned long> > Result;
	Result.reserve(Bindings.size());
	for(std::map<unsigned long,unsigned long>::const_iterator QQ=Bindings.begin(); QQ!=Bindings.end(); ++QQ ) {
		unsigned long TOKEN = QQ->first;
		assert( TOKEN <= 21 ); // max int value of Token
		unsigned long VALUE = QQ->second;
		assert( VALUE <= 127 ); // max const register
		Result.push_back( std::pair<unsigned long,unsigned long>(TOKEN,VALUE) );
	}
	return Result;
}































