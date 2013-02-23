#ifndef SYSTEM_COMMANDS_HPP

#define SYSTEM_COMMANDS_HPP
#include "command.h"
#include "section_constants.h"
#include <vector>

struct thread;
class section;

typedef void (*KickProc)(thread* pThread, section* pSection);
typedef void (*IdleProc)(thread* pThread, section* pSection);


// =================================================================================

// request to launch new section
struct CCommand_LaunchSection : public CCommand
{
	long			m_SectionClassID;
	long			m_Magic;
	unsigned long	m_Flags;
	long			m_Partner;
	IdleProc        m_pIdleProc;
	KickProc m_pKickProc;
	CCommand_LaunchSection( long in_SectionClassID, long in_Magic, unsigned long in_Flags=SF_NONE, long in_Partner=SECTION_ID_ERROR ) :
		m_SectionClassID(in_SectionClassID)
		,m_Magic(in_Magic)
		,m_Flags(in_Flags)
		,m_Partner(in_Partner)
		,m_pIdleProc(NULL)
		,m_pKickProc(NULL)
	{}
};

// result of previous CCommand_LaunchSection command
struct CCommand_LaunchSectionResult : public CCommand
{
	long			m_Magic;
	unsigned long	m_ErrorCode;
	long			m_NewSectionID;
	CCommand_LaunchSectionResult( long in_Magic, unsigned long in_ErrorCode, long in_NewSectionID ) :
		m_NewSectionID(in_NewSectionID)
		,m_ErrorCode(in_ErrorCode)
		,m_Magic(in_Magic)
	{}
};

// =================================================================================

struct CCommand_FindSection : public CCommand
{
	long	m_SectionClassID;
	long	m_Magic;
	CCommand_FindSection( long in_SectionClassID, long in_Magic ) :
		m_SectionClassID(in_SectionClassID)
		,m_Magic(in_Magic)
	{}
};

struct CCommand_FindSectionResult : public CCommand
{
	long				m_Magic;
	std::vector<long>	m_SectionIDs;
	CCommand_FindSectionResult( long in_Magic ) :
		m_Magic(in_Magic)
	{}
};

// =================================================================================

#define CCommand_QuitApplication CCommand_QuitSystem
// quit application

// =================================================================================

// Section asks multithreading info.
struct CCommand_QueryMTInfo : public CCommand
{
	long m_Magic;
	CCommand_QueryMTInfo(long in_Magic) : m_Magic(in_Magic) {}
};

// MT info itself.
struct CCommand_MTInfo : public CCommand
{
	long m_Magic;
	long m_NProcessors;
	long m_MaxThreads;
	long m_CurrThreads;
	CCommand_MTInfo(long in_Magic, long in_NProcessors, long in_MaxThreads, long in_CurrThreads) :
		m_Magic(in_Magic)
		,m_NProcessors(in_NProcessors)
		,m_MaxThreads(in_MaxThreads)
		,m_CurrThreads(in_CurrThreads)
	{}
};

// =================================================================================

// Launch additional threads.
// ATM, number of launched threads can only be increased. Not decreased.
struct CCommand_LaunchThreads : public CCommand
{
	long m_NThreads; // 
	long m_Magic;
	CCommand_LaunchThreads( long in_NThreads, long in_Magic ) :
		m_NThreads(in_NThreads)
		,m_Magic(in_Magic)
	{}
};

enum THREADS_LAUNCH_RESULT
{
	TLR_OK									= 0,
	TLR_TOO_MANY_THREADS_REQUESTED			= 1,
	TLR_SYSTEM_ERROR						= 2, // Total fuckup. The best response for this is to terminate the application.
	TLR_DECREASE_THREADS_NUM_UNSUPPORTED	= 3,
	TLR_FORCE_DWORD							= 0xffffffff
};

struct CCommand_LaunchThreadsResult : public CCommand
{
	long m_Magic;
	long m_Result;
	CCommand_LaunchThreadsResult(long in_Magic, long in_Result=TLR_OK) :
		m_Magic(in_Magic)
		,m_Result(in_Result)
	{}
};

// =================================================================================

struct CCommand_SetTimer_Request : public CCommand
{
	CCommand_SetTimer_Request( unsigned long in_MagicID, unsigned long in_TimeMicroseconds ) :
		m_MagicID(in_MagicID)
		,m_TimeMicroseconds(in_TimeMicroseconds)
	{}
	unsigned long m_MagicID;
	unsigned long m_TimeMicroseconds;
};

struct CCommand_SetTimer_Response : public CCommand
{
	enum SETTIMER_RESULT
	{
		SETTIMER_OK,
		SETTIMER_TOO_MANY_TIMERS,
		SETTIMER_ERROR
	};
	CCommand_SetTimer_Response( unsigned long in_MagicID, SETTIMER_RESULT in_Result ) :
		m_MagicID(in_MagicID)
		,m_Result(in_Result)
	{}
	unsigned long m_MagicID;
	SETTIMER_RESULT m_Result;
};

// TODO: CCommand_CancelTimer_Request + CCommand_CancelTimer_Response

struct CCommand_Timer_Event : public CCommand
{
	CCommand_Timer_Event( unsigned long in_MagicID ) :
		m_MagicID(in_MagicID)
	{}
	unsigned long m_MagicID;
};

// =================================================================================

struct CCommand_InputLocked : public CCommand
{
	CCommand_InputLocked(HANDLE in_hInputLock) : m_hInputLock(in_hInputLock) {}
	HANDLE m_hInputLock;
};

struct CCommand_InputUnlocked : public CCommand
{
	CCommand_InputUnlocked(HANDLE in_hInputLock) : m_hInputLock(in_hInputLock) {}
	HANDLE m_hInputLock;
};


// Called by the section when it creates window and window message loop.
struct CCommand_WindowCreated : public CCommand
{
	CCommand_WindowCreated(HWND in_HWnd) : m_HWnd(in_HWnd) {}
	HWND m_HWnd;
};

// Called by the section when it closes window and window message pump.
struct CCommand_WindowClosed : public CCommand
{
	CCommand_WindowClosed(HWND in_HWnd) : m_HWnd(in_HWnd) {}
	HWND m_HWnd;
};

// Called by the system,
// if window section fetches command and it's queue is empty.

struct CCommand_QuitSystem  : public CCommand
{
};


///////////////////////////////////////////////////////////////


#endif