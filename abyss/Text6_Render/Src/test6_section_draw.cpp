#include "../Include/test6_section.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

void
CTest6Section::Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd )
{
//CLog::Print("  CTest6Section::Reaction( const CRenderSection_NewFrame& )\n");
	++m_NFramesToRender;
	++m_NFrames;
	if(GetHWND()!=NULL)
	{
		unsigned long ThisFrameTime = GetTickCount();
		if( (ThisFrameTime-m_LastTimeFrame)>=1000 )
		{
			float FPS = 1000.0f*(float(m_NFrames))/(float(ThisFrameTime-m_LastTimeFrame));
			char Buffer[256];
			sprintf(Buffer,"Render test. FPS=%1.0f",FPS);
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

void
CTest6Section::Reaction( long in_SrcSectionID, const CTest6Section_Render& in_rCmd )
{
	if(GetHWND()!=NULL)
	{
		RenderFrame();
	}
}

void
CTest6Section::RenderFrame()
{
//CLog::Print("  CTest6Section::RenderFrame()\n");
	assert(GetHWND()!=NULL);
	assert(GetRenderSectionID()!=SECTION_ID_ERROR);
	if(m_NFramesToRender>0)
	{
		//CLog::Print("  m_NFramesToRender=%lu\n",m_NFramesToRender);
		{
			CVector CamOrg( 10.0f*sin(m_Alpha), 5.0f, 10.0f*cos(m_Alpha) );
			CVector CamDir( CVector(0,0,0) - CamOrg );
			CamDir.Normalize();
			CVector Tmp( CVector(0,10,0) - CamOrg );
			CVector CamRight( CamDir*Tmp );
			CamRight.Normalize();
			CTCommandSender<CRenderSection_SetCamera>::SendCommand(
				GetRenderSectionID(),
				CRenderSection_SetCamera(	CamOrg,CamDir,CamRight,
											CONST_PI_2, 0.75f*CONST_PI_2,
											0.01f, 100.0f)
			);
			m_Alpha += 0.001f;
			if(m_Alpha>CONST_2PI)
				m_Alpha -= CONST_2PI;
		}
		{
//			CTCommandSender<CRenderSection_RenderVB>::SendCommand(
//				GetRenderSectionID(),
//				CRenderSection_RenderVB(m_M0,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
//			);
			CTCommandSender<CRenderSection_RenderVB>::SendCommand(
				GetRenderSectionID(),
				CRenderSection_RenderVB(m_M0,m_VBHandle,m_PSHandle,0,m_PrimitiveType,m_NPrimitives)
			);
			//CTCommandSender<CRenderSection_RenderVB>::SendCommand(
			//	GetRenderSectionID(),
			//	CRenderSection_RenderVB(m_M1,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			//);
			//CTCommandSender<CRenderSection_RenderVB>::SendCommand(
			//	GetRenderSectionID(),
			//	CRenderSection_RenderVB(m_M2,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			//);
			//CTCommandSender<CRenderSection_RenderVB>::SendCommand(
			//	GetRenderSectionID(),
			//	CRenderSection_RenderVB(m_M3,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			//);
			//CTCommandSender<CRenderSection_RenderVB>::SendCommand(
			//	GetRenderSectionID(),
			//	CRenderSection_RenderVB(m_M4,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			//);
		}
		CTCommandSender<CRenderSection_PresentAndClear>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_PresentAndClear( 0 )
		);
		--m_NFramesToRender;
	}
}

