#ifndef SECTION_A_HPP_INCLUDED
#define SECTION_A_HPP_INCLUDED

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "section_a_interface.h"

#pragma warning(disable:4250)

class CSectionA :
	public CSection
	,public CTCommandSender<CSectionA_Bump>
	                                                      
	,public CTCommandSender<CCommand_LaunchSection>
	,public CTCommandSender<CCommand_QueryMTInfo>
	,public CTCommandSender<CCommand_LaunchThreads>
{
public:
	CSectionA( long in_SectionID );
	~CSectionA();
	void Reaction( const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( const CCommand_MTInfo& in_rMTInfoCommand );
	void Reaction( const CCommand_LaunchThreadsResult& in_rResult );
	void Reaction( const CSectionA_Bump& in_rBump );
	void Reaction( const CCommand_LaunchSectionResult& in_rResult );
	//long GetThidID() { return ; }
private:
	//unsigned long m_StartTime;
	long m_Counter;
};

#endif














