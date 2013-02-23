#ifndef WORKER_SECTION_INTERFACE_HPP_INCLUDED
#define WORKER_SECTION_INTERFACE_HPP_INCLUDED

#include "../../MT_Core/Include/command.h"

#define CWorkerSection_CLSID 845634

// ======================================================================================

struct CWorkerSection_RenderPicture : public CCommand
{
	CWorkerSection_RenderPicture( long in_X, long in_Y, long in_DX, long in_DY, long in_WindowWidth, long in_WindowHeight, long in_WindowSectionID, bool in_bLast ) :
		m_X(in_X)
		,m_Y(in_Y)
		,m_DX(in_DX)
		,m_DY(in_DY)
		,m_WindowWidth(in_WindowWidth)
		,m_WindowHeight(in_WindowHeight)
		,m_WindowSectionID(in_WindowSectionID)
		,m_bLast(in_bLast)
	{}
	long m_X;
	long m_Y;
	long m_DX;
	long m_DY;
	long m_WindowWidth;
	long m_WindowHeight;
	long m_WindowSectionID;
	bool m_bLast;
};

// ======================================================================================

struct CWorkerSection_PictureRendered : public CCommand
{
	CWorkerSection_PictureRendered(long in_WorkerID, bool in_bLast) :
		m_WorkerID(in_WorkerID),
		m_bLast(in_bLast)
	{}
	long m_WorkerID;
	bool m_bLast;
};

// ======================================================================================

struct CWorkerSection_PictureData : public CCommand
{
	CWorkerSection_PictureData( long in_X, long in_Y, long in_DX, long in_DY, long in_WindowWidth, long in_WindowHeight ) :
		m_X(in_X)
		,m_Y(in_Y)
		,m_DX(in_DX)
		,m_DY(in_DY)
		,m_WindowWidth(in_WindowWidth)
		,m_WindowHeight(in_WindowHeight)
	{}
	CWorkerSection_PictureData( long in_X, long in_Y, long in_DX, long in_DY, long in_WindowWidth, long in_WindowHeight, std::vector<unsigned long>& in_Data ) :
		m_X(in_X)
		,m_Y(in_Y)
		,m_DX(in_DX)
		,m_DY(in_DY)
		,m_WindowWidth(in_WindowWidth)
		,m_WindowHeight(in_WindowHeight)
		,m_Data(in_Data)
	{}
	long m_X;
	long m_Y;
	long m_DX;
	long m_DY;
	long m_WindowWidth;
	long m_WindowHeight;
	std::vector<unsigned long> m_Data;
};

// ======================================================================================

struct CWorkerSection_SetupScene : public CCommand
{
	CWorkerSection_SetupScene( long in_WindowWidth, long in_WindowHeight ) :
		m_WindowWidth(in_WindowWidth),
		m_WindowHeight(in_WindowHeight)
	{}
	long m_WindowWidth;
	long m_WindowHeight;
};

// ======================================================================================

struct CWorkerSection_SetupSceneComplete : public CCommand {};

// ======================================================================================
// ======================================================================================
// ======================================================================================
// ======================================================================================
// ======================================================================================
// ======================================================================================
// ======================================================================================

#endif
