#include "../Include/demo_section.h"
#include "../../MT_Core/Include/log.h"

#include <assert.h>

#define SUN_CAMERA_UPDATE_PERIOD 40

void
CDemoSection::Reaction(long in_SrcSectionID,const CDemoSection_StartRender& in_rCmd)
{
	m_NFramesToRender = 2;
	m_FirstTimeFrame = GetTickCount();
	if(GetHWND()!=NULL)
	{
		RenderFrame();
		RenderFrame();
	}
}

void
CDemoSection::Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& in_rResponse )
{
	assert( in_rResponse.m_Result==CCommand_SetTimer_Response::SETTIMER_OK );
	assert( in_rResponse.m_MagicID == 777 );
}

void
CDemoSection::Reaction( long in_SrcSectionID, const CCommand_Timer_Event& in_rEvent )
{
	assert( in_rEvent.m_MagicID == 777 );
	assert(SECTION_ID_ERROR!=m_SunSectionID);
	CDemoSection_WorkerCameraInform Cmd;
	Cmd.m_CameraRight = m_Right;
	Cmd.m_CameraUp = m_Up;
	CTCommandSender< CDemoSection_WorkerCameraInform >::SendCommand(
		m_SunSectionID
		,Cmd
	);
	m_LastCameraUpdateTime = GetTickCount();
}

void
CDemoSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerCameraInform_Response& in_rResponse)
{
	assert(SECTION_ID_ERROR!=m_SunSectionID);
	unsigned long T = GetTickCount();
	if( T-m_LastCameraUpdateTime > SUN_CAMERA_UPDATE_PERIOD )
	{
		CDemoSection_WorkerCameraInform Cmd;
		Cmd.m_CameraRight = m_Right;
		Cmd.m_CameraUp = m_Up;
		CTCommandSender< CDemoSection_WorkerCameraInform >::SendCommand(
			m_SunSectionID
			,Cmd
		);
		m_LastCameraUpdateTime = T;
	}
	else
	{
		CTCommandSender<CCommand_SetTimer_Request>::SendCommand( SECTION_ID_SYSTEM, CCommand_SetTimer_Request(777,(T-m_LastCameraUpdateTime)*1000) );
	}
}

void
CDemoSection::Reaction(long in_SrcSectionID,const CDemoSection_WorkerUpdate& in_rUpdate)
{
	CStoredInfo::iterator It = m_StoredInfo.find( in_SrcSectionID );
	assert( It != m_StoredInfo.end() );
//	a
//	CLog::Println( "Expected update size %d " , (*It).second.size()) ;
//	CLog::Println( "Real update size %d " , in_rUpdate.m_Positions.size());
	assert( (*It).second.size() >= in_rUpdate.m_Positions.size() );
	for(size_t i=0; i<in_rUpdate.m_Positions.size(); ++i)
	{
		(*It).second[i].m_Position = in_rUpdate.m_Positions[i];
	}
}

void
CDemoSection::Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd )
{
	++m_NFramesToRender;
	++m_NFrames;
	if(GetHWND()!=NULL)
	{
		unsigned long ThisFrameTime = GetTickCount();
		if( (ThisFrameTime-m_LastTimeFrame)>=CLOCKS_PER_SEC )
		{
			float FPS = 1000.0f*(float(m_NFrames))/(float(ThisFrameTime-m_LastTimeFrame));
			char Buffer[256];
			sprintf(Buffer,"Test 2. FPS=%1.0f",FPS);
			CTCommandSender<CWindowSection_WindowName>::SendCommand(
				GetWindowSectionID(),
				CWindowSection_WindowName(std::string(Buffer))
			);
			//
			m_LastTimeFrame = ThisFrameTime;
			m_NFrames=0;
		}
		RenderFrame();

	}
}

#define FLAT_ANGLE_VELOCITY (CONST_2PI/20.0f)
#define VERT_ANGLE_VELOCITY (CONST_2PI/8.0f)
#define CAMERA_DIST 50.0f

void
CDemoSection::RenderFrame()
{

	assert(GetHWND()!=NULL);
	assert(GetRenderSectionID()!=SECTION_ID_ERROR);
	if(m_NFramesToRender>0)
	{
		// set camera
		unsigned long dT = GetTickCount()-m_FirstTimeFrame;
		float dTSeconds = float(dT)/1000.0f;
		float FlatAngle = dTSeconds*FLAT_ANGLE_VELOCITY;
		float VertAngle = dTSeconds*VERT_ANGLE_VELOCITY;
		VertAngle = CONST_PI_4*sin(VertAngle);
		CVector CameraOrg(
			cos(VertAngle)*sin(-FlatAngle),
			sin(VertAngle),
			cos(VertAngle)*cos(FlatAngle)
		);
		CameraOrg *= CAMERA_DIST;
		CVector CameraDir = CVector(0,0,0)-CameraOrg;
		CameraDir.Normalize();
		CVector CameraRight( CameraDir*CVector(0,1,0) );
		CameraRight.Normalize();

		m_Up = CameraRight*CameraDir;
		m_Right = CameraRight;

		CTCommandSender<CRenderSection_SetCamera>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_SetCamera(
				CameraOrg,		// org
				CameraDir,		// dir
				CameraRight,	// right
				CONST_PI_3, 0.75f*CONST_PI_3,	// view angles
				0.01f,	// min Z
				100.0f	// max Z
			)
		);
		// set lights
		CTCommandSender<CRenderSection_SetAmbient>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_SetAmbient( 0x808080 )
		);
		//CTCommandSender<CRenderSection_SetDirectionalLight>::SendCommand(
		//	GetRenderSectionID(),
		//	CRenderSection_SetDirectionalLight( CVector(-1.0f,-1.0f,-1.0f), 1.0f, 1.0f, 1.0f )
		//);
		CTCommandSender<CRenderSection_SetPointLight>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_SetPointLight( CVector(0.0f,0.0f,0.0f), 1.0f, 1.0f, 1.0f )
		);
		// render stars
		CMatrix CameraCentered( CameraOrg, CVector(0,0,1), CVector(0,1,0) );
		for(CStoredInfo::const_iterator It=m_StoredInfo.begin(); It!=m_StoredInfo.end(); ++It)
		{
			const std::vector<CRenderable>& rRenderables = (*It).second;
			for(std::vector<CRenderable>::const_iterator Itt=rRenderables.begin(); Itt!=rRenderables.end(); ++Itt)
			{
				const CRenderable& rObject = *Itt;
				if(OT_CENTERED_ON_CAMERA==rObject.m_Type)
				{
					CTCommandSender<CRenderSection_RenderVB>::SendCommand(
						GetRenderSectionID(),
						CRenderSection_RenderVB(
							CameraCentered,
							rObject.m_VBHandle,
							rObject.m_PSHandle,
							0,
							rObject.m_PrimitiveType,
							rObject.m_PrimitiveCount
						)
					);
				}
			}
		}
		// render solids
		for(CStoredInfo::const_iterator It=m_StoredInfo.begin(); It!=m_StoredInfo.end(); ++It)
		{
			const std::vector<CRenderable>& rRenderables = (*It).second;
			for(std::vector<CRenderable>::const_iterator Itt=rRenderables.begin(); Itt!=rRenderables.end(); ++Itt)
			{
				const CRenderable& rObject = *Itt;
				if(OT_SOLID==rObject.m_Type)
				{
					CTCommandSender<CRenderSection_RenderVB>::SendCommand(
						GetRenderSectionID(),
						CRenderSection_RenderVB(
							rObject.m_Position,
							rObject.m_VBHandle,
							rObject.m_PSHandle,
							0,
							rObject.m_PrimitiveType,
							rObject.m_PrimitiveCount
						)
					);
				}
			}
		}
		// render translucent
		for(CStoredInfo::const_iterator It=m_StoredInfo.begin(); It!=m_StoredInfo.end(); ++It)
		{
			const std::vector<CRenderable>& rRenderables = (*It).second;
			for(std::vector<CRenderable>::const_iterator Itt=rRenderables.begin(); Itt!=rRenderables.end(); ++Itt)
			{
				const CRenderable& rObject = *Itt;
				if(OT_TRANSLUCENT==rObject.m_Type)
				{
					CTCommandSender<CRenderSection_RenderVB>::SendCommand(
						GetRenderSectionID(),
						CRenderSection_RenderVB(
							rObject.m_Position,
							rObject.m_VBHandle,
							rObject.m_PSHandle,
							0,
							rObject.m_PrimitiveType,
							rObject.m_PrimitiveCount
						)
					);
				}
			}
		}
		// text
		CTCommandSender<CRenderSection_RenderString>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_RenderString( 100, 50, m_pChars, 1, 0xc0ffffff )
		);
		// Dot
		CTCommandSender<CRenderSection_RenderDot>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_RenderDot( 10, 10, 0xc0ff0000 )
		);
		// present
		CTCommandSender<CRenderSection_PresentAndClear>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_PresentAndClear( 0 )
		);
		--m_NFramesToRender;
	}
}

void CDemoSection::Reaction(long in_SrcSectionID , const CInputConsoleSection_KeyboardUpdate& update ) 
{
	if (update.m_bPressed) 
	{
		if (m_pCharPointer > 50 ) 
		{
			m_pCharPointer =0;
			memset(m_pChars,0,100);
		}

		TCHAR              strLetter[10];
		 wsprintf( strLetter, TEXT("0x%02x "), update.m_bScanCode);
	    for (int i= 0 ; i < strlen(strLetter) ; i ++ ) 
		{
			m_pChars[m_pCharPointer] = strLetter[i];
			m_pCharPointer ++;
		}
		
		
	}
}

void CDemoSection::Reaction(long in_SrcSectionID , const CInputConsoleSection_MouseUpdate& update ) 
{
	if (update.m_bLeftPressed )
	{
		CTCommandSender<CRenderSection_RenderDot>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_RenderDot( update.m_dwX, update.m_dwY, 0xc0ff0000 )
		);
		CTCommandSender<CRenderSection_RenderDot>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_RenderDot( update.m_dwX+1, update.m_dwY+1, 0xc0ff0000 )
		);
		CTCommandSender<CRenderSection_RenderDot>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_RenderDot( update.m_dwX+2, update.m_dwY+2, 0xc0ff0000 )
		);
		CTCommandSender<CRenderSection_RenderDot>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_RenderDot( update.m_dwX+2, update.m_dwY, 0xc0ff0000 )
		);
		CTCommandSender<CRenderSection_RenderDot>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_RenderDot( update.m_dwX, update.m_dwY+2, 0xc0ff0000 )
		);


	}
}















