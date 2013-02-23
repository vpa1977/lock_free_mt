#ifndef DEMO_SECTION_INTERFACE_HPP_INCLUDED
#define DEMO_SECTION_INTERFACE_HPP_INCLUDED

#define DemoSection_CLSID 633670120

#include "../../MT_Core/Include/command.h"
#include "../../Render_DX8/Include/algebra.h"
#include <vector>
#include <string>

// ================================================================================================================

struct CDemoSection_WorkerInit_Request: public CCommand 
{
	CDemoSection_WorkerInit_Request( const std::string& in_rConfig, long in_VFSSectionID, long in_PreloaderSectionID, long in_RenderSectionID ) :
		m_ConfigFileForThisSection(in_rConfig)
		,m_VFSSectionID(in_VFSSectionID)
		,m_PreloaderSectionID(in_PreloaderSectionID)
		,m_RenderSectionID(in_RenderSectionID)
		//,m_Slave(false)

	{}
	std::string m_ConfigFileForThisSection;
	long m_VFSSectionID;
	long m_PreloaderSectionID;
	long m_RenderSectionID;
};

enum OBJECT_TYPE
{
	OT_CENTERED_ON_CAMERA,
	OT_TRANSLUCENT,
	OT_SOLID
};

struct CDemoSection_WorkerInit_Response: public CCommand 
{
	
	bool m_bSuccess;
	struct CRenderable
	{
		CRenderable( OBJECT_TYPE in_Type=OT_SOLID ) :
			m_Type(in_Type)
		{}
		unsigned long m_VBHandle;
		unsigned long m_PSHandle;
		unsigned long m_PrimitiveType;
		unsigned long m_PrimitiveCount;
		CMatrix       m_InitialPosition;
		OBJECT_TYPE   m_Type;
	};
	std::vector< CRenderable > m_Renderables;
};

// ================================================================================================================

struct CDemoSection_WorkerCameraInform : public CCommand
{
	CVector m_CameraRight;
	CVector m_CameraUp;
};

struct CDemoSection_WorkerCameraInform_Response : public CCommand
{
};

// ================================================================================================================

struct CDemoSection_WorkerUpdate: public CCommand 
{
	std::vector< CMatrix > m_Positions;
};

// ================================================================================================================

struct CDemoSection_StartRender : public CCommand {};

// ================================================================================================================

struct CDemoSection_WorkerShutdown_Request : public CCommand {};

struct CDemoSection_WorkerShutdown_Response : public CCommand {};

// ================================================================================================================

#endif























