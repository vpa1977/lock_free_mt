#ifndef SECTION_B_HPP_INCLUDED
#define SECTION_B_HPP_INCLUDED

#include "../MT_Core/Include/section.h"
#include "../MT_Core/Include/command_sender.h"
#include "section_b_interface.h"

#pragma warning(disable:4250)

class CSectionB :
	public CSection,
	public CTCommandSender<CSectionB_Bump>
{
public:
	CSectionB( long in_SectionID );
	~CSectionB();
	void Reaction( const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( const CSectionB_Bump& in_rBump );
};

#endif














