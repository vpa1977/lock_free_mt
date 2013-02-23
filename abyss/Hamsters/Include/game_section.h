#ifndef GAME_HPP
#define GAME_HPP
#include "hamsters.h"
#include "hamster_section_interface.h"
#include "state_controller_section.h"
#include "../../preloader/include/section_preloader_interface.h"
#include "../include/field.h"


#define LOAD_FIELD_MAGIC 1
#define LOAD_HAMSTER_MAGIC 2
#define LOAD_SPLASHER_MAGIC 3


class CGameSection : public CState
	, public CTCommandSender<CPreloaderSection_ModelLoadRequest>
	, public CTCommandSender<CRenderSection_RenderObject_Request>
	, public CTCommandSender<CRenderSection_SetCamera>
	, public CTCommandSender<CRenderSection_SetAmbient>
	, public CTCommandSender<CPreloaderSection_MatrixLoadRequest>
	
{
public:
	CGameSection( long inSectionID) : CState(inSectionID) 
	{
		REGISTER_REACTION(	CGameSection, CPreloaderSection_ModelLoadResponse);
		REGISTER_REACTION(	CGameSection, CPreloaderSection_MatrixLoadResponse);
		REGISTER_REACTION( 	CGameSection, CRenderSection_RenderObject_Response);
		m_pShadowCreator = new CRenderDX8_ShadowCreator;
	}
	virtual ~CGameSection()
	{
		delete m_pShadowCreator;
	}
	void SetRenderSectionID(long id){
		m_RenderSectionID = id;
	}
	void SetPreloaderID(long id)
	{
		m_PreloaderID = id;
	}
	void SetVFSID(long id)
	{
		m_VFSSection = id;
	}
	virtual std::string GetName()
	{
		return "GAME";
	}
	void Reaction(long src, const CPreloaderSection_ModelLoadResponse& in );
	void Reaction(long src , const CRenderSection_RenderObject_Response& in);
	void Reaction(long src, const CPreloaderSection_MatrixLoadResponse& in );
protected:

	virtual void UpdateState(long inSectionID,const CState_UpdateRequest& update);
	/** 
		Initialization of resources which are needed for lifetime is done here
	*/
	virtual void InitState(long inSectionID,const CState_Init& update);
	/**
		Allocate temporary resources, free them before invoking EndState
	*/
	virtual void Allocate(long inSectionID,const CState_AllocateResourcesRequest& update);
	/**
		Invoked when we dispose of section
	*/
	virtual void Release(long inSectionID,const CState_ReleaseResourcesRequest& update);
private:
	long m_RenderSectionID;
	long m_PreloaderID;
	long m_VFSSection;
	
	
	CShadowCreator* m_pShadowCreator;
	boost::shared_ptr<CField> m_pField;
	boost::shared_ptr<CHamsterFactory> m_pFactory;
	// just a temp queue which holds object which should be rendered 
	std::deque<CObject*> m_RequestQueue;
};

#endif