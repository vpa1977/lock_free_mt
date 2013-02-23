#ifndef WORKER_SECTION_HPP_INCLUDED
#define WORKER_SECTION_HPP_INCLUDED

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "test3_interface.h"
#include "worker_section_interface.h"

#pragma warning(disable:4250)

class CWorkerSection :
	public CSection
	,public CTCommandSender<CCommand_QuitSection>
	,public CTCommandSender<CWorkerSection_SetupSceneComplete>
	,public CTCommandSender<CWorkerSection_PictureRendered>
	,public CTCommandSender<CWorkerSection_PictureData>
{
public:
	CWorkerSection( long in_SectionID );
	~CWorkerSection();
	std::string GetName()
	{
		return "CWorkerSection";
	}
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CWorkerSection_SetupScene& in_rSetup );
	void Reaction( long in_SrcSectionID, const CWorkerSection_RenderPicture& in_rRenderPicture );
	void Reaction( long in_SrcSectionID, const CTest3Section_ApplicationShutdown& in_rShutdown );
private:
};

#endif
