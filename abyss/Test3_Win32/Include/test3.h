#ifndef TEST3_HPP_INCLUDED
#define TEST3_HPP_INCLUDED

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "../../App_Win32/Include/window_section_interface.h"
#include "test3_interface.h"
#include "worker_section_interface.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
using namespace ::boost;
using namespace ::boost::multi_index;

#include <list>

#pragma warning(disable:4250)

class CTest3Section :
	public CSection
	,public CTCommandSender<CCommand_QuitSection>
	,public CTCommandSender<CCommand_QuitApplication>
	,public CTCommandSender<CCommand_LaunchSection>
	,public CTCommandSender<CCommand_FindSection>
	,public CTCommandSender<CCommand_QueryMTInfo>
	,public CTCommandSender<CCommand_LaunchThreads>
	,public CTCommandSender<CWindowSection_CreateWindow>
	,public CTCommandSender<CWindowSection_CloseWindow>
	,public CTCommandSender<CWindowSection_WindowParams>
	,public CTCommandSender<CWindowSection_WindowName>
	,public CTCommandSender<CWindowSection_TestCommand>
	,public CTCommandSender<CTest3Section_ApplicationShutdown>
	,public CTCommandSender<CWorkerSection_SetupScene>
	,public CTCommandSender<CWorkerSection_RenderPicture>
{
public:
	CTest3Section( long in_SectionID );
	~CTest3Section();
	std::string GetName()
	{
		return "CTest3Section";
	}
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CCommand_MTInfo& in_rMTInfoCommand );
	void Reaction( long in_SrcSectionID, const CCommand_LaunchThreadsResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CCommand_FindSectionResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CWindowSection_CreateWindowResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CWindowSection_CloseWindowResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CWindowSection_WindowParamsResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CWindowSection_WindowClosed& in_rCommand );
	void Reaction( long in_SrcSectionID, const CWindowSection_Char& in_rCommand );
	void Reaction( long in_SrcSectionID, const CWorkerSection_SetupSceneComplete& in_rCommand );
	void Reaction( long in_SrcSectionID, const CWorkerSection_PictureRendered& in_rCommand );
private:
	long m_WindowSectionID;
	bool m_bWindowCreated;
	DWORD m_StartTime;

	struct CWorkerInfo
	{
		CWorkerInfo(long in_Magic) :
			m_Magic(in_Magic)
			,m_SectionID(SECTION_ID_ERROR)
			,m_bRunning(false)
		{}
		CWorkerInfo(long in_Magic, long in_SectionID, bool in_bRunning) :
			m_Magic(in_Magic)
			,m_SectionID(in_SectionID)
			,m_bRunning(in_bRunning)
		{}
		long m_Magic;
		long m_SectionID;
		bool m_bRunning;
	};

	class Tag_Magic {};
	typedef ordered_unique
	<
		tag< Tag_Magic >,
		member< CWorkerInfo,long,&CWorkerInfo::m_Magic >
	> CIndexMagic;

	class Tag_SectionID {};
	typedef ordered_non_unique
	<
		tag< Tag_SectionID >,
		member< CWorkerInfo,long,&CWorkerInfo::m_SectionID >
	> CIndexSectionID;

	class Tag_Running {};
	typedef ordered_non_unique
	<
		tag< Tag_Running >,
		member< CWorkerInfo,bool,&CWorkerInfo::m_bRunning >
	> CIndexRunning;

	typedef multi_index_container
	<
		CWorkerInfo,
		indexed_by
		<
			CIndexMagic
			,CIndexSectionID
			,CIndexRunning
		>
	> CWorkersMap;
	typedef CWorkersMap::index< Tag_Magic     >::type CWorkerMap_by_Magic;
	typedef CWorkersMap::index< Tag_SectionID >::type CWorkerMap_by_SectionID;
	typedef CWorkersMap::index< Tag_Running   >::type CWorkerMap_by_Running;

	struct AssignSectionIDAndRunningFlag
	{
		AssignSectionIDAndRunningFlag(long in_SectionID) :
			m_SectionID(in_SectionID)
		{}
		void operator()(CTest3Section::CWorkerInfo& inout_rWorkerInfo)
		{
			inout_rWorkerInfo.m_SectionID = m_SectionID;
			inout_rWorkerInfo.m_bRunning = true;
		}
	private:
		long m_SectionID;
	};

	unsigned long m_NWorkers;
	unsigned long m_NProcessors;
	CWorkersMap m_WorkersMap;

	void CheckWorkersAndWindow();
	void StartWorking();
	bool m_bWorking;

	struct CChunkPosition
	{
		CChunkPosition(long in_X, long in_Y) : m_X(in_X), m_Y(in_Y) {}
		long m_X;
		long m_Y;
	};
	std::list<CChunkPosition> m_Chunks;
};


#endif



















