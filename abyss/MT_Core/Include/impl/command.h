#ifndef COMMAND_HPP

#define COMMAND_HPP

//#include "allocator.h"
#include "../Log.h"

class section;
struct chunk;

// abstract command
struct abstract_command
{
};
typedef abstract_command CCommand;

struct message
{
	long m_clock;
	long m_source;
	long m_destination;
	long m_type;
	abstract_command* pCommand;
	chunk* pAllocator;
};

typedef void (*ConformFunc)(section* in_pSection, long in_SrcSectionID, const abstract_command* in_pCommand);

template< typename SEC, typename CMD >
class CTConform
{
public:
	static void Conform(section* in_pSection, long in_SrcSectionID, const abstract_command* in_pCommand)
	{
		
		if (((SEC*)in_pSection)->m_running_flag.get()) 
		{
			CLog::Print( "Executing Command %s \n", typeid(CMD).name() );
			((SEC*)in_pSection)->Reaction( in_SrcSectionID, *((const CMD*)in_pCommand) );
		}
		else
		{
			CLog::Print( "Skipping Command %s \n", typeid(CMD).name() );
		}

		((const CMD*)in_pCommand)->~CMD();
		 
	}
};








struct CCommand_SectionStartup : public CCommand
{
	long m_ParentSectionID;
	long m_ThisSectionID;
	CCommand_SectionStartup( long in_ParentSectionID, long in_ThisSectionID ) :
		m_ParentSectionID(in_ParentSectionID)
		,m_ThisSectionID(in_ThisSectionID)
	{}
};

struct CCommand_SectionQuit : public CCommand
{
};



#endif
