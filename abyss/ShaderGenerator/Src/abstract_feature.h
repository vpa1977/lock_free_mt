#ifndef ABSTRACT_FEATURE_HPP_DEFINED
#define ABSTRACT_FEATURE_HPP_DEFINED

#include "shader_resources.h"
#include "input_params.h"
#include "linker_interface.h"
#include <vector>
#include <list>
#include <string>

class CAbstractFeature
{
public:
	virtual ~CAbstractFeature() {}

	virtual void AddFragments( INVLink* in_pLinker ) = 0;
	virtual void GetMyIDs( INVLink* in_pLinker ) = 0;
	// 
	virtual void Reset() = 0;

	// Feature requests something from other features.
	virtual bool PreGenerate( CShaderResources& rRes, const CInputParams& rParams ) = 0;

	// generate
	virtual std::list<NVLINK_FRAGMENT_ID> GetFragmentsForThisCombination( const CShaderResources& rRes, const CInputParams& rParams ) = 0;
	// post-generate
	virtual void GetConstants( INVLink* in_pLinker ) = 0;

	virtual std::vector< std::pair<unsigned long,unsigned long> > GetBindings() const = 0;

protected:
	void AddFragmentsInternal( INVLink* in_pLinker, const char* in_pFileName );

};

#endif
































