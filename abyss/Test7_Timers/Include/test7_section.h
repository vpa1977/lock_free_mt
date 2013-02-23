#ifndef SECTION_A_HPP_INCLUDED
#define SECTION_A_HPP_INCLUDED

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "test7_section_interface.h"

#pragma warning(disable:4250)


struct TestCommandA : public CCommand
{
};

class CTest7Section :
	public CSection
	,public CTCommandSender<CCommand_LaunchSection>
	,public CTCommandSender<CCommand_QueryMTInfo>
	,public CTCommandSender<CCommand_LaunchThreads>
	//
	,public CTCommandSender<CCommand_SetTimer_Request>
	,public CTCommandSender<TestCommandA>
{
public:
	CTest7Section( long in_SectionID );
	~CTest7Section();
	std::string GetName()
	{
		return "CTest7Section";
	}
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CCommand_MTInfo& in_rMTInfoCommand );
	void Reaction( long in_SrcSectionID, const CCommand_LaunchThreadsResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult );
	void Reaction( long in_SrcSectionID, const TestCommandA& in_rResult );
	//
	void Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& in_rResponse );
	void Reaction( long in_SrcSectionID, const CCommand_Timer_Event& in_rEvent );
private:
};

#endif
