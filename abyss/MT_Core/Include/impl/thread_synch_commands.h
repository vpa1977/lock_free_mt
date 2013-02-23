#ifndef THREAD_SYNCH_COMMANDS_HPP

#define THREAD_SYNCH_COMMANDS_HPP

#include "command.h"

//////////////////////Service command

struct CCommand_ThreadSync_SectionStart : public CCommand 
{
	section* m_psection;
	long src_id;
	long m_Magic;
};
struct CCommand_ThreadSync_SectionStartResponse : public CCommand
{
	section* m_psection;
	long src_id;
	long m_Magic;
};

struct CCommand_ThreadSync_SectionQuit : public CCommand
{
	long m_thread_id;
	section* m_psection;
};

struct CCommand_ThreadSync_SectionQuitResponse : public CCommand
{
	long m_thread_id;
	section* m_psection;
};




#endif