#ifndef SECTION_DIRECTINPUT_HPP

#define SECTION_DIRECTINPUT_HPP


#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"

#include "section_directinput_interface.h"


class CDirectInputSection : public CSection

{
	public:
		CDirectInputSection(long sectionID );
		~CDirectInputSection();
	
		void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
		{
			
		};

		virtual std::string GetName() {return "CDirectInputSection"; };

	
};




#endif 

