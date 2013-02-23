#ifndef FEATURE_POSITION_AND_BONES_HPP_INCLUDED
#define FEATURE_POSITION_AND_BONES_HPP_INCLUDED

#include "abstract_feature.h"

class CFeature_PositionBones : public CAbstractFeature
{
public:
	CFeature_PositionBones();
	~CFeature_PositionBones();

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

private:

	CShaderResources	m_Res;
	CInputParams		m_Params;

	NVLINK_FRAGMENT_ID	m_Fragment_Bones[5];
	NVLINK_FRAGMENT_ID	m_Fragment_Bones_Normal[5];
	NVLINK_FRAGMENT_ID	m_Fragment_Position_WorldViewProj;

	NVLINK_CONSTANT_ID	m_Constant_10xx;
	DWORD				m_Slot_10xx;

	NVLINK_CONSTANT_ID	m_Constant_WorldViewProj;
	DWORD				m_Slot_WorldViewProj;

	//NVLINK_FRAGMENT_ID m_WorldViewProj_0Bones_DirectOutput;
	//NVLINK_CONSTANT_ID m_WorldViewProj_ConstantID;
	//DWORD m_MatrixSlot;
};


#endif




































