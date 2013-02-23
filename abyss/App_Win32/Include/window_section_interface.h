#ifndef WINDOW_SECTION_INTERFACE_HPP_INCLUDED
#define WINDOW_SECTION_INTERFACE_HPP_INCLUDED

#include "../../MT_Core/Include/command.h"
#include <string>

#define CWindowSection_CLSID 0xdeadbeef

// ============================================================================================

enum WINDOW_STYLE
{
	WS_RESIZEABLE	= 0, // resizeable window with minimize and maximize options
	WS_NONE			= WS_RESIZEABLE,
	WS_MINMAX		= 1, // non-resizeable window with minimize and maximize options
	WS_CONSTANT		= 2, // non-resizeable window
	WS_FULLSCREEN	= 3  // popup window (for fullscreen rendering)
};

struct CWindowSection_CreateWindow : public CCommand
{
	CWindowSection_CreateWindow( WINDOW_STYLE in_WS, long in_ClientAreaWidth, long in_ClientAreaHeight, std::string in_Name, long in_MagicID ) :
		m_WS(in_WS)
		,m_ClientAreaWidth(in_ClientAreaWidth)
		,m_ClientAreaHeight(in_ClientAreaHeight)
		,m_Name(in_Name)
		,m_MagicID(in_MagicID)
	{}
	WINDOW_STYLE m_WS;
	long m_ClientAreaWidth;
	long m_ClientAreaHeight;
	std::string m_Name;
	long m_MagicID;	// m_MagicID>0. Used as window ID in CWindowSection_WindowClosed() notification.
};

enum CREATE_WINDOW_RESULT
{
	CWR_OK				= 0,
	CWR_FAUL			= 1, // system error
	CWR_PARAMETERS		= 2, // wrong parameters
	CWR_MANY_WINDOWS	= 3, // only one window at a time
	CWR_PREVIOUS_WINDOW_IS_DIEING	= 4 // Previous window destruction is still pending. Wait for CWindowSection_WindowClosed.
};

struct CWindowSection_CreateWindowResult : public CCommand
{
	CWindowSection_CreateWindowResult( unsigned long in_Result, long in_MagicID, HWND in_HWnd=NULL ) :
		m_Result(in_Result)
		,m_MagicID(in_MagicID)
		,m_HWnd(in_HWnd)
	{}
	unsigned long m_Result;
	long m_MagicID;
	HWND m_HWnd;
};

// ============================================================================================

enum CLOSE_WINDOW_RESULT
{
	CLWR_OK				= 0,
	CLWR_ERR			= 1, // system error
	CLWR_NO_WINDOW		= 2, // no window yet created
	CLWR_WRONG_HWND		= 3, // wrong HWnd
	CLWR_PREVIOUS_WINDOW_IS_DIEING	= 4 // Previous window destruction is still pending.
};

struct CWindowSection_CloseWindow : public CCommand
{
	CWindowSection_CloseWindow( HWND in_HWnd ) :
		m_HWnd(in_HWnd)
	{}
	HWND m_HWnd;
};

struct CWindowSection_CloseWindowResult : public CCommand
{
	CWindowSection_CloseWindowResult( unsigned long in_Result, HWND in_HWnd ) :
		m_Result(in_Result)
		,m_HWnd(in_HWnd)
	{}
	unsigned long m_Result;
	HWND m_HWnd;
};

// ============================================================================================

struct CWindowSection_WindowParams : public CCommand
{
	CWindowSection_WindowParams(	WINDOW_STYLE in_WS,
									long in_ClientAreaWidth, long in_ClientAreaHeight,
									HWND in_HWnd ) :
		m_WS(in_WS)
		,m_ClientAreaWidth(in_ClientAreaWidth)
		,m_ClientAreaHeight(in_ClientAreaHeight)
		,m_HWnd(in_HWnd)
	{}
	WINDOW_STYLE m_WS;
	long m_ClientAreaWidth;
	long m_ClientAreaHeight;
	HWND m_HWnd;
};

enum WINDOW_PARAMS_RESULT
{
	WPR_OK							= 0,
	WPR_ERR							= 1, // system error
	WPR_NO_WINDOW					= 2, // no window created yet
	WPR_PARAMS						= 3, // wrong parameters
	WPR_PREVIOUS_WINDOW_IS_DIEING	= 4  // Previous window destruction is still pending.
};

struct CWindowSection_WindowParamsResult : public CCommand
{
	CWindowSection_WindowParamsResult( unsigned long in_Result, HWND in_HWnd ) :
		m_Result(in_Result)
		,m_HWnd(in_HWnd)
	{}
	unsigned long m_Result;
	HWND m_HWnd;
};

// ============================================================================================

struct CWindowSection_WindowName : public CCommand
{
	CWindowSection_WindowName(std::string& in_Name) : m_Name(in_Name) {}
	std::string m_Name;
};

// ============================================================================================

struct CWindowSection_WindowClosed : public CCommand
{
	CWindowSection_WindowClosed(HWND in_HWnd) : m_HWnd(in_HWnd) {}
	HWND m_HWnd;
};

// ============================================================================================

struct CWindowSection_Char : public CCommand
{
	CWindowSection_Char( char in_Ch, HWND in_HWnd ) : m_Ch(in_Ch), m_HWnd(in_HWnd) {}
	char m_Ch;
	HWND m_HWnd;
};

// ============================================================================================

struct CWindowSection_TestCommand : public CCommand {};

// ============================================================================================

struct CWindowSection_Activated : public CCommand
{
	CWindowSection_Activated( bool in_Active, HWND in_HWnd ) : m_Active(in_Active), m_HWnd(in_HWnd) {}
	bool m_Active;
	HWND m_HWnd;
};

// ============================================================================================

struct CWindowSection_WindowSize : public CCommand
{
	CWindowSection_WindowSize( bool in_bMinimized, long in_Width, long in_Height, HWND in_HWnd ) :
		m_bMinimized(in_bMinimized)
		,m_Width(in_Width)
		,m_Height(in_Height)
		,m_HWnd(in_HWnd)
	{}
	bool m_bMinimized;
	long m_Width;
	long m_Height;
	HWND m_HWnd;
};

// ============================================================================================

#endif









