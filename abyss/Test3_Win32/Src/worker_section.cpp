#include "../Include/worker_section.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/log.h"

#include "../Raytracer/raytracer.h"
#include "../Raytracer/scene.h"

#include <math.h>

Raytracer::Engine* g_Renderer = NULL;

static CTSectionRegistrator<CWorkerSection_CLSID,CWorkerSection> g_CWorkerSectionRegistrator( MULTIPLE_INSTANCES_ALLOWED );

CWorkerSection::CWorkerSection( long in_SectionID ) :
	CSection( in_SectionID )
{
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL),			CTConform<CWorkerSection,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CWorkerSection_SetupScene*)NULL),			CTConform<CWorkerSection,CWorkerSection_SetupScene>::Conform );
	RegisterReaction( GetID((CWorkerSection_RenderPicture*)NULL),		CTConform<CWorkerSection,CWorkerSection_RenderPicture>::Conform );
	RegisterReaction( GetID((CTest3Section_ApplicationShutdown*)NULL),	CTConform<CWorkerSection,CTest3Section_ApplicationShutdown>::Conform );
	m_MaxTick = 1;
}

CWorkerSection::~CWorkerSection()
{
}

void
CWorkerSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
}

void
CWorkerSection::Reaction( long in_SrcSectionID, const CWorkerSection_SetupScene& in_rSetup )
{
	// setup scene
	assert(!g_Renderer);
	g_Renderer = new Raytracer::Engine();
	g_Renderer->GetScene()->InitScene();
	g_Renderer->SetTarget( in_rSetup.m_WindowWidth, in_rSetup.m_WindowHeight );
	g_Renderer->InitRender( Raytracer::vector3( -2, 0, -2 ), Raytracer::vector3( 0, 0.8f, 5 ) );
	//
	CTCommandSender<CWorkerSection_SetupSceneComplete>::SendCommand(
		SECTION_ID_BROADCAST,
		CWorkerSection_SetupSceneComplete()
	);
}

void
CWorkerSection::Reaction( long in_SrcSectionID, const CWorkerSection_RenderPicture& in_rRenderPicture )
{
	CLog::Print("CWorkerSection::Reaction( const CWorkerSection_RenderPicture& in_rRenderPicture )\n");
	assert(g_Renderer);
	// render
	CWorkerSection_PictureData PictureData(
		in_rRenderPicture.m_X,
		in_rRenderPicture.m_Y,
		in_rRenderPicture.m_DX,
		in_rRenderPicture.m_DY,
		in_rRenderPicture.m_WindowWidth,
		in_rRenderPicture.m_WindowHeight
	);
	PictureData.m_Data.resize( in_rRenderPicture.m_DX*in_rRenderPicture.m_DY );
	unsigned long* pColorData = &(PictureData.m_Data.front());

	for(long Y=in_rRenderPicture.m_Y; Y<in_rRenderPicture.m_Y+in_rRenderPicture.m_DY; ++Y)
	{
		for(long X=in_rRenderPicture.m_X; X<in_rRenderPicture.m_X+in_rRenderPicture.m_DX; ++X)
		{
			unsigned long CCC = g_Renderer->Render(X,Y);
			*(pColorData++) = CCC;
		}
	}

	// request for more rendering
	CTCommandSender<CWorkerSection_PictureRendered>::SendCommand(
		in_SrcSectionID,
		CWorkerSection_PictureRendered( GetThisID(), in_rRenderPicture.m_bLast )
	);
	// rendering results to display
	CTCommandSender<CWorkerSection_PictureData>::SendCommand(
		in_rRenderPicture.m_WindowSectionID,
		PictureData
	);
	CLog::Print("CWorkerSection::Reaction( const CWorkerSection_RenderPicture& in_rRenderPicture ) end\n");
}

void
CWorkerSection::Reaction( long in_SrcSectionID, const CTest3Section_ApplicationShutdown& in_rShutdown )
{
	CTCommandSender<CCommand_QuitSection>::SendCommand( SECTION_ID_SYSTEM, CCommand_QuitSection() );
}






