#ifndef FEATURE_NORMAL_AND_LIGHTS_HPP_INCLUDED
#define FEATURE_NORMAL_AND_LIGHTS_HPP_INCLUDED

#include "abstract_feature.h"
#include <boost\\shared_ptr.hpp>
#include <map>

using namespace ::boost;

class CActiveFragment {
public:
	virtual ~CActiveFragment() {}

	virtual std::string GetName() = 0;

	virtual void GetIDs(INVLink* in_pLinker) = 0;

	virtual bool BelongsToCurrentSetup(const CShaderResources& rRes, const CInputParams& rParams) const = 0;
	virtual NVLINK_FRAGMENT_ID GetID() const = 0;

	virtual void GetConstants(INVLink* in_pLinker) = 0;
	virtual std::map< unsigned long,unsigned long > GetBindings() const = 0;
};

class CActiveFragment_LightDisabled : public CActiveFragment {
public:
	std::string GetName();
	void GetIDs(INVLink* in_pLinker);
	bool BelongsToCurrentSetup(const CShaderResources& rRes, const CInputParams& rParams) const { return rParams.m_LightEnabled == false; }
	NVLINK_FRAGMENT_ID GetID() const { return m_FragmentID_Disabled; }
	void GetConstants(INVLink* in_pLinker);
	std::map< unsigned long,unsigned long > GetBindings() const;
private:
	NVLINK_FRAGMENT_ID m_FragmentID_Disabled;
	NVLINK_CONSTANT_ID m_ConstantID_WhiteColor;
	DWORD m_Slot_WhiteColor;
};

class CActiveFragment_Light : public CActiveFragment {
public:
	CActiveFragment_Light(long in_N = -1, CInputParams::LIGHT_TYPE in_Type = CInputParams::LT_NONE );
	std::string GetName();
	void GetIDs(INVLink* in_pLinker);
	bool BelongsToCurrentSetup(const CShaderResources& rRes, const CInputParams& rParams) const;
	NVLINK_FRAGMENT_ID GetID() const;
	void GetConstants(INVLink* in_pLinker);
	std::map< unsigned long,unsigned long > GetBindings() const;
private:
	long m_N;
	CInputParams::LIGHT_TYPE m_Type;

	std::string m_FragmentName;
	NVLINK_FRAGMENT_ID m_FragmentID;

	std::string m_LightOrgName;
	NVLINK_CONSTANT_ID m_ConstantID_LightOrg;
	DWORD m_Slot_LightOrg;

	std::string m_LightDirName;
	NVLINK_CONSTANT_ID m_ConstantID_LightDir;
	DWORD m_Slot_LightDir;

	std::string m_LightColorName;
	NVLINK_CONSTANT_ID m_ConstantID_LightColor;
	DWORD m_Slot_LightColor;

	std::string m_LightParamsName;
	NVLINK_CONSTANT_ID m_ConstantID_LightParams;
	DWORD m_Slot_LightParams;

	NVLINK_CONSTANT_ID m_ConstantID_WhiteColor;
	DWORD m_Slot_WhiteColor;
};

class CActiveFragment_Output : public CActiveFragment {
public:
	CActiveFragment_Output(bool in_bDiffuseVertexColorPresent);
	std::string GetName();
	void GetIDs(INVLink* in_pLinker);
	bool BelongsToCurrentSetup(const CShaderResources& rRes, const CInputParams& rParams) const;
	NVLINK_FRAGMENT_ID GetID() const;
	void GetConstants(INVLink* in_pLinker);
	std::map< unsigned long,unsigned long > GetBindings() const;
private:
	bool m_bDiffusePresent;
	NVLINK_FRAGMENT_ID m_FragmentID;
};

class CFeature_Lighting : public CAbstractFeature
{
public:
	CFeature_Lighting();
	~CFeature_Lighting();

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
	std::vector< boost::shared_ptr<CActiveFragment> > m_Fragments;
	std::vector< boost::shared_ptr<CActiveFragment> > m_ActiveFragments;

	CShaderResources	m_Res;
	CInputParams		m_Params;
};


#endif




































