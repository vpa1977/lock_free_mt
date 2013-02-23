#include "../Include/render_section.h"
#include "../Include/rendercritical_section_interface.h"

#include "../Include/es_static_mesh.h"

#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>


CRenderSection::CRenderSection( long in_SectionID ) :
	CSection( in_SectionID )
	//,m_RenderCriticalSectionID(SECTION_ID_ERROR)
	//,m_WindowSectionID(SECTION_ID_ERROR)
	//,m_HWnd(NULL)
	//,m_bTerminatingDueToWindowClosed(false)
	//,m_SectionThatUssuedWindowClosed(SECTION_ID_ERROR)
	//,m_b3DInfoRequested(false)
	//,m_3DInfoRequesterID(SECTION_ID_ERROR)
	//,m_bInitRequested(false)
	//,m_InitRequesterID(SECTION_ID_ERROR)
	,m_bLost(false)
	,m_bInsideResetAttempt(false)
{
	//
	RegisterReaction(	GetID((CCommand_SectionStartup*)NULL),	CTConform<CRenderSection,CCommand_SectionStartup>::Conform );
	RegisterReaction(	GetID((CCommand_LaunchSectionResult*)NULL),	CTConform<CRenderSection,CCommand_LaunchSectionResult>::Conform );
	//
	RegisterReaction(	GetID((CRenderSection_BindToWindow*)NULL),	CTConform<CRenderSection,CRenderSection_BindToWindow>::Conform );
	RegisterReaction(	GetID((CRenderSection_WindowClosed*)NULL),	CTConform<CRenderSection,CRenderSection_WindowClosed>::Conform );
	RegisterReaction(	GetID((CRenderSection_Get3DEnvironmentInfo*)NULL),	CTConform<CRenderSection,CRenderSection_Get3DEnvironmentInfo>::Conform );
	RegisterReaction(	GetID((CRenderSection_InitRender*)NULL),	CTConform<CRenderSection,CRenderSection_InitRender>::Conform );
	RegisterReaction(	GetID((CRenderSection_CloseRender*)NULL),	CTConform<CRenderSection,CRenderSection_CloseRender>::Conform );
	RegisterReaction(	GetID((CRenderSection_CloseRenderResult*)NULL),	CTConform<CRenderSection,CRenderSection_CloseRenderResult>::Conform );
	RegisterReaction(	GetID((CRenderSection_PresentAndClear*)NULL),	CTConform<CRenderSection,CRenderSection_PresentAndClear>::Conform );
	RegisterReaction(	GetID((CRenderSection_SetCamera*)NULL),	CTConform<CRenderSection,CRenderSection_SetCamera>::Conform );
	RegisterReaction(	GetID((CRenderSection_SetAmbient*)NULL),	CTConform<CRenderSection,CRenderSection_SetAmbient>::Conform );
	RegisterReaction(	GetID((CRenderSection_SetDirectionalLight*)NULL),	CTConform<CRenderSection,CRenderSection_SetDirectionalLight>::Conform );
	RegisterReaction(	GetID((CRenderSection_SetPointLight*)NULL),	CTConform<CRenderSection,CRenderSection_SetPointLight>::Conform );
	RegisterReaction(	GetID((CRenderSection_CreateVertexBuffer_Request*)NULL),	CTConform<CRenderSection,CRenderSection_CreateVertexBuffer_Request>::Conform );
	RegisterReaction(	GetID((CRenderSection_UpdateVertexBuffer_Request*)NULL),	CTConform<CRenderSection,CRenderSection_UpdateVertexBuffer_Request>::Conform );
	RegisterReaction(	GetID((CRenderSection_ReleaseVertexBuffer_Request*)NULL),	CTConform<CRenderSection,CRenderSection_ReleaseVertexBuffer_Request>::Conform );
	RegisterReaction(	GetID((CRenderSection_CreateTexture_Request*)NULL),			CTConform<CRenderSection,CRenderSection_CreateTexture_Request>::Conform );
	RegisterReaction(	GetID((CRenderSection_ReleaseTexture_Request*)NULL),		CTConform<CRenderSection,CRenderSection_ReleaseTexture_Request>::Conform );
	RegisterReaction(	GetID((CRenderSection_CreatePixelShader_Request*)NULL),			CTConform<CRenderSection,CRenderSection_CreatePixelShader_Request>::Conform );
	RegisterReaction(	GetID((CRenderSection_ReleasePixelShader_Request*)NULL),		CTConform<CRenderSection,CRenderSection_ReleasePixelShader_Request>::Conform );
	RegisterReaction(	GetID((CRenderSection_CreateFont_Request*)NULL),			CTConform<CRenderSection,CRenderSection_CreateFont_Request>::Conform );
	RegisterReaction(	GetID((CRenderSection_RenderTestObject*)NULL),	CTConform<CRenderSection,CRenderSection_RenderTestObject>::Conform );
	RegisterReaction(	GetID((CRenderSection_RenderVB*)NULL),	CTConform<CRenderSection,CRenderSection_RenderVB>::Conform );
	RegisterReaction(	GetID((CRenderSection_RenderString*)NULL),	CTConform<CRenderSection,CRenderSection_RenderString>::Conform );
	RegisterReaction(	GetID((CRenderSection_RenderDot*)NULL),	CTConform<CRenderSection,CRenderSection_RenderDot>::Conform );
	REGISTER_REACTION(	CRenderSection,	CRenderSection_RenderObject_Request	);
	//
	RegisterReaction(	GetID((CRenderCriticalSection_BoundToWindow*)NULL),	CTConform<CRenderSection,CRenderCriticalSection_BoundToWindow>::Conform );
	RegisterReaction(	GetID((CRenderCriticalSection_3DEnvironmentInfo*)NULL),	CTConform<CRenderSection,CRenderCriticalSection_3DEnvironmentInfo>::Conform );
	RegisterReaction(	GetID((CRenderCriticalSection_InitRenderResult*)NULL),	CTConform<CRenderSection,CRenderCriticalSection_InitRenderResult>::Conform );
	RegisterReaction(	GetID((CRenderCriticalSection_RestoreDeviceComplete*)NULL),	CTConform<CRenderSection,CRenderCriticalSection_RestoreDeviceComplete>::Conform );
}

CRenderSection::~CRenderSection()
{
}

// ====================================================================================================================

void
CRenderSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CLog::Print("CRenderSection::Reaction( const CCommand_SectionStartup& in_rStartupCommand )\n");
	CLog::Print("  ID = %ld\n",GetThisID());
	m_Layer2.Initialize(this);
	m_Layer1.Initialize(this,&m_Layer2);
	m_Layer0.Initialize(this,&m_Layer1);
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rLaunchResult )
{
	CLog::Print("CRenderSection::Reaction( const CCommand_LaunchSectionResult& in_rLaunchResult )\n");
	assert(RENDER_CRITICAL_LAUNCH_MAGIC==in_rLaunchResult.m_Magic);
	if(m_Layer0.IsActive())
	{
		m_Layer0.Received_SectionLaunchResult( (SLR_OK==in_rLaunchResult.m_ErrorCode)? in_rLaunchResult.m_NewSectionID : SECTION_ID_ERROR );
	}
}

// ====================================================================================================================

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_BindToWindow& in_rBind )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_BindToWindow& in_rBind )\n");
	if(m_Layer0.IsActive())
	{
		m_Layer0.Received_BindToWindow( in_SrcSectionID, in_rBind.m_WindowSectionID, in_rBind.m_HWnd );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_WindowClosed& in_rWindowClosed )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_WindowClosed& in_rWindowClosed )\n");
	if(m_Layer0.IsActive())
	{
		CLog::Print("  thisID = %ld\n",GetThisID());
		CLog::Print("  srcID  = %ld\n",in_SrcSectionID);
		Render.ReleaseEverything();
		m_Layer0.Received_UnbindFromWindow( in_rWindowClosed.m_HWnd );
		CTCommandSender<CRenderSection_WindowClosedCleanupComplete>::SendCommand(
			in_SrcSectionID,
			CRenderSection_WindowClosedCleanupComplete( in_rWindowClosed.m_HWnd )
		);
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_Get3DEnvironmentInfo& in_rGetInfo )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_Get3DEnvironmentInfo& in_rGetInfo )\n");
	if(m_Layer1.IsActive())
	{
		CLog::Print("  thisID = %ld\n",GetThisID());
		CLog::Print("  srcID  = %ld\n",in_SrcSectionID);
		m_Layer1.Received_Get3DInfo(in_SrcSectionID);
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_InitRender& in_rInit )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_InitRender& in_rInit )\n");
	if(m_Layer1.IsActive())
	{
		m_Layer1.Received_InitRender( in_SrcSectionID, in_rInit );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_CloseRender& in_rClose )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_CloseRender& in_rClose )\n");
	if(m_Layer1.IsActive())
	{
		Render.ReleaseEverything();
		m_Layer1.Received_CloseRender( in_SrcSectionID );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_CloseRenderResult& in_rResult )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_CloseRenderResult& in_rResult )\n");
	if(m_Layer1.IsActive())
	{
		m_Layer1.Received_CloseRenderResult( in_rResult );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_PresentAndClear& in_rCmd )
{
//CLog::Print("  CRenderSection::Reaction( const CRenderSection_PresentAndClear& )\n");
	if(m_Layer2.IsActive())
	{
		if(!m_bLost)
		{
			if(Render.Present())
			{
				Render.Clear(in_rCmd.m_Color);
			}
			else
			{
				Render.PrepareToRestoreDevice();
				m_bLost = true;
				m_Layer2.Received_DeviceLost();
				m_bInsideResetAttempt = true;
			}
		}
		else if(!m_bInsideResetAttempt)
		{
			m_Layer2.Received_DeviceLost();
			m_bInsideResetAttempt = true;
		}
		CTCommandSender<CRenderSection_NewFrame>::SendCommand(
			in_SrcSectionID,
			CRenderSection_NewFrame()
		);
	}
	//CLog::Print("CRenderSection::Reaction( const CRenderSection_PresentAndClear& in_rCmd ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_SetCamera& in_rCmd )
{
//CLog::Print("  CRenderSection::Reaction( const CRenderSection_SetCamera& )\n");
	if(m_Layer2.IsActive())
	{
		Render.SetCamera(	in_rCmd.m_Org,in_rCmd.m_Dir,in_rCmd.m_Right,
							in_rCmd.m_FovX,in_rCmd.m_FovY,
							in_rCmd.m_Znear,in_rCmd.m_Zfar);
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_SetAmbient& in_rCmd )
{
	if(m_Layer2.IsActive())
	{
		Render.SetAmbient( in_rCmd.m_Ambient );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_SetDirectionalLight& in_rCmd )
{
	if(m_Layer2.IsActive())
	{
		Render.SetDirectionalLight( in_rCmd.m_Dir, in_rCmd.m_R, in_rCmd.m_G, in_rCmd.m_B );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_SetPointLight& in_rCmd )
{
	if(m_Layer2.IsActive())
	{
		Render.SetPointLight( in_rCmd.m_Org, in_rCmd.m_R, in_rCmd.m_G, in_rCmd.m_B );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Request& in_rCmd )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_CreateVertexBuffer_Request& in_rCmd )\n");
	if(m_Layer2.IsActive())
	{
		const std::vector<unsigned char>& rVBData = in_rCmd.m_Data;
		assert( !rVBData.empty() );
		unsigned long Handle = Render.CreateVertexBuffer(
			&( rVBData[0] ),
			(unsigned long)rVBData.size(),
			in_rCmd.m_VBFormat,
			in_rCmd.m_bDynamic
		);
		CTCommandSender<CRenderSection_CreateVertexBuffer_Response>::SendCommand(
			in_SrcSectionID,
			CRenderSection_CreateVertexBuffer_Response( Handle, in_rCmd.m_Magic )
		);
	}
	CLog::Print("CRenderSection::Reaction( const CRenderSection_CreateVertexBuffer_Request& in_rCmd ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_UpdateVertexBuffer_Request& in_rCmd )
{
//	CLog::Print("CRenderSection::Reaction( const CRenderSection_UpdateVertexBuffer_Request& )\n");
	if(m_Layer2.IsActive())
	{
		const std::vector<unsigned char>& rVBData = in_rCmd.m_Data;
		assert( !rVBData.empty() );
		bool Ok = Render.UpdateVertexBuffer(
			in_rCmd.m_VBHandle,
			&( rVBData[0] ),
			(unsigned long)rVBData.size()
		);
		CTCommandSender<CRenderSection_UpdateVertexBuffer_Response>::SendCommand(
			in_SrcSectionID,
			CRenderSection_UpdateVertexBuffer_Response( Ok, in_rCmd.m_VBHandle )
		);
	}
//	CLog::Print("CRenderSection::Reaction( const CRenderSection_UpdateVertexBuffer_Request& ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_ReleaseVertexBuffer_Request& in_rCmd )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_ReleaseVertexBuffer_Request& in_rCmd )\n");
	if(m_Layer2.IsActive())
	{
		Render.ReleaseVertexBuffer( in_rCmd.m_VBHandle );
	}
	CLog::Print("CRenderSection::Reaction( const CRenderSection_ReleaseVertexBuffer_Request& in_rCmd ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Request& in_rCmd )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_CreateTexture_Request& in_rCmd )\n");
	if(m_Layer2.IsActive())
	{
		const std::vector<unsigned char>& rTexData = in_rCmd.m_Data;
		assert( !rTexData.empty() );
		unsigned long Handle = Render.CreateTexture(
			in_rCmd.m_DX
			,in_rCmd.m_DY
			,in_rCmd.m_HasAlpha
			,&( rTexData[0] )
			,(unsigned long)rTexData.size()
		);
		CTCommandSender<CRenderSection_CreateTexture_Response>::SendCommand(
			in_SrcSectionID,
			CRenderSection_CreateTexture_Response( Handle, in_rCmd.m_Magic )
		);
	}
	CLog::Print("CRenderSection::Reaction( const CRenderSection_CreateTexture_Request& in_rCmd ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_ReleaseTexture_Request& in_rCmd )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_ReleaseTexture_Request& in_rCmd )\n");
	if(m_Layer2.IsActive())
	{
		Render.ReleaseTexture( in_rCmd.m_TexHandle);
	}
	CLog::Print("CRenderSection::Reaction( const CRenderSection_ReleaseTexture_Request& in_rCmd ) end\n");
}


void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Request& in_rCmd )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_CreatePixelShader_Request& in_rCmd )\n");
	if(m_Layer2.IsActive())
	{
		unsigned long Handle = Render.CreatePixelShader( in_rCmd.m_ShaderDescription );
		CTCommandSender<CRenderSection_CreatePixelShader_Response>::SendCommand(
			in_SrcSectionID,
			CRenderSection_CreatePixelShader_Response( Handle, in_rCmd.m_Magic )
		);
	}
	CLog::Print("CRenderSection::Reaction( const CRenderSection_CreatePixelShader_Request& in_rCmd ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_ReleasePixelShader_Request& in_rCmd )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_ReleasePixelShader_Request& in_rCmd )\n");
	if(m_Layer2.IsActive())
	{
		Render.ReleasePixelShader( in_rCmd.m_ShaderHandle );
	}
	CLog::Print("CRenderSection::Reaction( const CRenderSection_ReleasePixelShader_Request& in_rCmd ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateFont_Request& in_rCmd )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_CreateFont_Request& )\n");
	if(m_Layer2.IsActive())
	{
		unsigned long NewFontHandle = Render.CreateFont( in_rCmd.m_FontDescription );
		CTCommandSender<CRenderSection_CreateFont_Response>::SendCommand(
			in_SrcSectionID,
			CRenderSection_CreateFont_Response( NewFontHandle, in_rCmd.m_Magic )
		);
	}
	CLog::Print("CRenderSection::Reaction( const CRenderSection_CreateFont_Request& ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_RenderTestObject& in_rCmd )
{
	if(m_Layer2.IsActive())
	{
		Render.DrawTestObject( in_rCmd.m_M );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_RenderVB& in_rCmd )
{
//CLog::Print("  CRenderSection::Reaction( const CRenderSection_RenderVB& )\n");
	if(m_Layer2.IsActive())
	{
		Render.DrawVB( in_rCmd.m_M, in_rCmd.m_VBHandle, in_rCmd.m_PixelShaderHandle, in_rCmd.m_FirstVertex, in_rCmd.m_PrimitiveType, in_rCmd.m_PrimitiveCount );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_RenderString& in_rCmd )
{
//CLog::Print("  CRenderSection::Reaction( const CRenderSection_RenderString& )\n");
	if(m_Layer2.IsActive())
	{
		Render.DrawString( in_rCmd.m_X, in_rCmd.m_Y, in_rCmd.m_String.c_str(), in_rCmd.m_FontHandle, in_rCmd.m_Color );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_RenderDot& in_rCmd )
{
//CLog::Print("  CRenderSection::Reaction( const CRenderSection_RenderDot& )\n");
	if(m_Layer2.IsActive())
	{
		Render.DrawDot( in_rCmd.m_X, in_rCmd.m_Y, in_rCmd.m_Color );
	}
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_RenderObject_Request& in_rCmd )
{
//CLog::Print("  CRenderSection::Reaction( const CRenderSection_RenderObject_Request& )\n");
	if(m_Layer2.IsActive()) {
		CShadow* pShadow = in_rCmd.m_pShadow;
		RenderShadowRecurse( in_rCmd.m_pShadow, CMatrix() );
		CRenderSection_RenderObject_Response Resp;
		Resp.m_pShadows.push_back(pShadow);
		CTCommandSender<CRenderSection_RenderObject_Response>::SendCommand( in_SrcSectionID, Resp );
	}
}

// ====================================================================================================================

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_BoundToWindow& in_rBound )
{
	CLog::Print("CRenderSection::Reaction( const CRenderCriticalSection_BoundToWindow& in_rBound )\n");
	if(m_Layer0.IsActive())
	{
		m_Layer0.Received_RenderCriticalBoundToWindow();
	}
	CLog::Print("CRenderSection::Reaction( const CRenderCriticalSection_BoundToWindow& in_rBound ) done\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_3DEnvironmentInfo& in_rInfo )
{
	CLog::Print("CRenderSection::Reaction( const CRenderCriticalSection_3DEnvironmentInfo& in_rInfo )\n");
	if(m_Layer1.IsActive())
	{
		m_Layer1.Received_3DInfo( in_rInfo );
	}
	CLog::Print("CRenderSection::Reaction( const CRenderCriticalSection_3DEnvironmentInfo& in_rInfo ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_InitRenderResult& in_rResult )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_InitRenderResult& in_rResult )\n");
	if(m_Layer1.IsActive())
	{
		m_bLost = false;
		m_bInsideResetAttempt = false;
		if(in_rResult.m_Result==IRR_OK)
		{
			Render.SetInterface( in_rResult.m_pD3D, in_rResult.m_pDevice, in_rResult.m_RenderMode, in_rResult.m_VertexProcessingType );
		}
		m_Layer1.Received_InitRenderResult( in_rResult );
	}
	CLog::Print("CRenderSection::Reaction( const CRenderSection_InitRenderResult& in_rResult ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_RestoreDeviceComplete& in_rResult )
{
	CLog::Print("CRenderSection::Reaction( const CRenderCriticalSection_RestoreDeviceComplete& in_rResult )\n");
	if(m_Layer2.IsActive())
	{
		m_bLost = (false==in_rResult.m_bSuccess);
		m_bInsideResetAttempt = false;
		if(!m_bLost)
			Render.RestoreResources();
	}
	CLog::Print("CRenderSection::Reaction( const CRenderCriticalSection_RestoreDeviceComplete& in_rResult ) end\n");
}

// ====================================================================================================================


void
CRenderSection::RenderShadowRecurse( CShadow* in_pShadow, const CMatrix& in_CumulativeParentPosition ) {
	assert( in_pShadow );
	CMatrix ThisMatrix( in_pShadow->m_Position.Get() * in_CumulativeParentPosition );
	switch( in_pShadow->GetType() ) {
		case ET_STATIC_MESH:
			{
				CEntityShadow_StaticMesh* pShadowStaticMesh = (CEntityShadow_StaticMesh*)in_pShadow;
				CStaticMesh_DX8& rStaticMeshDX8( *(pShadowStaticMesh->m_pStaticMeshDX8) );
				Render.DrawRenderables(
					ThisMatrix,
					rStaticMeshDX8.m_pStaticMesh->GetMaterials(),
					rStaticMeshDX8.m_pStaticMesh->GetRemappedGeometries(),
					rStaticMeshDX8.m_Materials,
					rStaticMeshDX8.m_BoneRemaps,
					rStaticMeshDX8.m_Skeleton
				);
			}
			break;
		default:
			break;
	}
	for( size_t N=0; N<in_pShadow->m_Children.size(); ++N ) {
		RenderShadowRecurse( in_pShadow->m_Children[N], ThisMatrix );
	}
}




/*

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_WindowClosedCleanupComplete& in_rCommand )
{
	CLog::Print("CRenderSection::Reaction( const CRenderCriticalSection_WindowClosedCleanupComplete& in_rCommand )\n");
	CLog::Print("  thisID   = %ld\n",GetThisID());
	CLog::Print("  callerID = %ld\n",m_SectionThatUssuedWindowClosed);
	assert(m_bTerminatingDueToWindowClosed);
	assert(m_SectionThatUssuedWindowClosed != SECTION_ID_ERROR);
	assert(m_RenderCriticalSectionID==in_SrcSectionID);
	assert(m_HWnd==in_rCommand.m_HWnd);
	//
	Send_WindowClosedCleanupComplete();
	//
	CLog::Print("CRenderSection::Reaction( const CRenderCriticalSection_WindowClosedCleanupComplete& in_rCommand ) end\n");
}

void
CRenderSection::Send_WindowClosedCleanupComplete()
{
	assert(m_bTerminatingDueToWindowClosed);
	assert(m_SectionThatUssuedWindowClosed != SECTION_ID_ERROR);
	CTCommandSender<CRenderSection_WindowClosedCleanupComplete>::SendCommand(
		m_SectionThatUssuedWindowClosed,
		CRenderSection_WindowClosedCleanupComplete( m_HWnd )
	);
	m_bTerminatingDueToWindowClosed = false;
	m_SectionThatUssuedWindowClosed = SECTION_ID_ERROR;
	m_HWnd = NULL;
}

// ====================================================================================================================

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderSection_Get3DEnvironmentInfo& in_rGetInfo )
{
	CLog::Print("CRenderSection::Reaction( const CRenderSection_Get3DEnvironmentInfo& in_rGetInfo )\n");
	if(!m_bTerminatingDueToWindowClosed)
	{
		if(SECTION_ID_ERROR==m_RenderCriticalSectionID)
		{
			m_WindowSectionID = in_rGetInfo.m_WindowSectionID;
			m_HWnd = in_rGetInfo.m_HWnd;
			bool R = CTCommandSender<CCommand_LaunchSection>::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_LaunchSection(
					CRenderCriticalSection_CLSID,
					RENDER_CRITICAL_LAUNCH_MAGIC,
					SF_BOUND|SF_EXCLUSIVE|SF_PARTNER,
					m_WindowSectionID
				)
			);
			assert(R);
			m_b3DInfoRequested = true;
			m_3DInfoRequesterID = in_SrcSectionID;
		}
		else
		{
			Get3DInfo(m_HWnd);
		}
	}
	CLog::Print("CRenderSection::Reaction( const CRenderSection_Get3DEnvironmentInfo& in_rGetInfo ) end\n");
}

void
CRenderSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_3DEnvironmentInfo& in_rInfo )
{
	CLog::Print("CRenderSection::Reaction( const CRenderCriticalSection_3DEnvironmentInfo& in_rInfo )\n");
	if(!m_bTerminatingDueToWindowClosed)
	{
		CRenderSection_3DEnvironmentInfo Info;
		Info.m_bError = in_rInfo.m_bError;
		if(!Info.m_bError)
		{
			Info.m_CanRenderWindowed = in_rInfo.m_CanRenderWindowed;
			Info.m_DisplayModes = in_rInfo.m_DisplayModes;
		}
		assert(m_3DInfoRequesterID!=SECTION_ID_ERROR);
		CTCommandSender<CRenderSection_3DEnvironmentInfo>::SendCommand(
			m_3DInfoRequesterID,
			Info
		);
	}
	m_b3DInfoRequested = false;
	CLog::Print("CRenderSection::Reaction( const CRenderCriticalSection_3DEnvironmentInfo& in_rInfo ) end\n");
}

void
CRenderSection::Get3DInfo(HWND in_HWnd)
{
	CLog::Print("  CRenderSection::Get3DInfo()\n");
	CTCommandSender<CRenderCriticalSection_Get3DEnvironmentInfo>::SendCommand(
		m_RenderCriticalSectionID,
		CRenderCriticalSection_Get3DEnvironmentInfo( in_HWnd )
	);
	CLog::Print("  CRenderSection::Get3DInfo() end\n");
}
*/






