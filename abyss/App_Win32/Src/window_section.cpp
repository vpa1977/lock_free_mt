#include "../Include/window_section.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/Log.h"
#include <assert.h>


thread* pCurrentThread;
void CWindowSectionKickProc(thread* pThread, section* pSection)
{
	CLog::Print("Kicking windows thread\n");
	((CWindowSection*)pSection)->KickProcImpl();
}

void CWindowSectionIdleProc(thread* pThread, section* pSection)
{
//	CLog::Print("CWindowSection::Reaction( const CCommand_WindowSectionBump& )\n");
	
	((CWindowSection*)pSection)->IdleProcImpl();
}


extern HINSTANCE	g_hInstance;
extern int			g_CmdShow;

static CTSectionRegistrator<CWindowSection_CLSID,CWindowSection> g_CWindowSectionRegistrator(SINGLE_INSTANCE_ONLY,REGULAR_SECTION,SF_BOUND);

void Touch_WindowSection() {}

CWindowSection* CWindowSection::m_pInstance = NULL;

CWindowSection::CWindowSection( long in_SectionID ) :
	CSection(in_SectionID)
	,m_MagicID(0)
	,m_Handle(NULL)
	,m_PreviousHandle(NULL)
	,m_bResizing(false)
	,m_LastWidth(-1)
	,m_LastHeight(-1)
	,m_bNameChanged(false)
{
	assert( !m_pInstance );
	m_pInstance = this;
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL),	CTConform<CWindowSection,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CWindowSection_CreateWindow*)NULL),CTConform<CWindowSection,CWindowSection_CreateWindow>::Conform );
	RegisterReaction( GetID((CWindowSection_CloseWindow*)NULL),	CTConform<CWindowSection,CWindowSection_CloseWindow>::Conform );
	RegisterReaction( GetID((CWindowSection_WindowParams*)NULL),CTConform<CWindowSection,CWindowSection_WindowParams>::Conform );
	//RegisterReaction( GetID((CCommand_WindowSectionBump*)NULL),	CTConform<CWindowSection,CCommand_WindowSectionBump>::Conform );
	RegisterReaction( GetID((CWindowSection_WindowName*)NULL),	CTConform<CWindowSection,CWindowSection_WindowName>::Conform );
	RegisterReaction( GetID((CWindowSection_TestCommand*)NULL),	CTConform<CWindowSection,CWindowSection_TestCommand>::Conform );
}

CWindowSection::~CWindowSection()
{
	m_pInstance = NULL;
}

const char* g_WindowClassName = "AbyssEngineWindow_CLS";

void
CWindowSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CLog::Print("CWindowSection::Reaction( const CCommand_SectionStartup& in_rStartupCommand )\n");
	CLog::Print("  ID = %ld\n",GetThisID());
	WNDCLASSEX wcex;
	memset( &wcex, 0, sizeof(WNDCLASSEX) );
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)COLOR_APPWORKSPACE;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_WindowClassName;
	bool qq = RegisterClassEx(&wcex) != 0;
	assert(qq);

	CLog::Print("CWindowSection::Reaction( const CCommand_SectionStartup& in_rStartupCommand ) end\n");
}

void
CWindowSection::Reaction( long in_SrcSectionID, const CWindowSection_CreateWindow& in_rCreateWindow )
{
	CLog::Print("CWindowSection::Reaction( const CWindowSection_CreateWindow& in_rCreateWindow )\n");
	assert(in_rCreateWindow.m_MagicID>0);
//	CLog::Print("CWindowSection -1-\n");
	if(m_PreviousHandle)
	{
		CLog::Print("CWindowSection -1.1-\n");
		CTCommandSender<CWindowSection_CreateWindowResult>::SendCommand(
			in_SrcSectionID,
			CWindowSection_CreateWindowResult(CWR_PREVIOUS_WINDOW_IS_DIEING,in_rCreateWindow.m_MagicID)
		);
	}
	else if(m_Handle)
	{
		CLog::Print("CWindowSection -1.2-\n");
		CTCommandSender<CWindowSection_CreateWindowResult>::SendCommand(
			in_SrcSectionID,
			CWindowSection_CreateWindowResult( CWR_MANY_WINDOWS, in_rCreateWindow.m_MagicID )
		);
	}
	else if(in_rCreateWindow.m_ClientAreaWidth<=0 || in_rCreateWindow.m_ClientAreaHeight<=0)
	{
		CLog::Print("CWindowSection -1.3-\n");
		CTCommandSender<CWindowSection_CreateWindowResult>::SendCommand(
			in_SrcSectionID,
			CWindowSection_CreateWindowResult( CWR_PARAMETERS, in_rCreateWindow.m_MagicID )
		);
	}
	else
	{
//		CLog::Print("CWindowSection -2-\n");
		DWORD Style;
		long X,Y;
		long DX,DY;
		GetStylePositionAndDimensions(	// input
										in_rCreateWindow.m_WS,
										in_rCreateWindow.m_ClientAreaWidth,in_rCreateWindow.m_ClientAreaHeight,
										// output
										Style,
										X,Y,
										DX,DY
									);
//		CLog::Print("CWindowSection -2.1-\n");
		m_Handle = CreateWindow(	g_WindowClassName,
									in_rCreateWindow.m_Name.c_str(),
									Style,
									X,Y,
									DX,DY,
									NULL,
									NULL,
									g_hInstance,
									NULL
								);
//		CLog::Print("CWindowSection -2.2-\n");
		if(m_Handle)
		{
//			CLog::Print("CWindowSection -3-\n");
			m_MagicID = in_rCreateWindow.m_MagicID;
			ShowWindow(m_Handle, g_CmdShow);
			UpdateWindow(m_Handle);
//			CLog::Print("CWindowSection -3.1-\n");
			CTCommandSender<CCommand_WindowCreated>::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_WindowCreated(m_Handle)
			);
//			CLog::Print("CWindowSection -3.2-\n");
			//
		}
		//else
		//{
		//	LPVOID pMessage;
		//	FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		//					NULL,
		//					GetLastError(),
		//					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		//					(LPTSTR)&pMessage,
		//					0,
		//					NULL );
		//	LocalFree( pMessage );
		//}
//		CLog::Print("CWindowSection -3.14- SrcSection = %ld\n",in_SrcSectionID);
		CTCommandSender<CWindowSection_CreateWindowResult>::SendCommand(
			in_SrcSectionID,
			CWindowSection_CreateWindowResult(
				(m_Handle==NULL) ? CWR_FAUL:CWR_OK,
				in_rCreateWindow.m_MagicID,
				m_Handle
			)
		);
		InstallThreadProcs( & CWindowSectionIdleProc , CWindowSectionKickProc);
		// Window section's message queue is empty at this point.
		// If we created window successfully, and called CTCommandSender<CCommand_WindowCreated>::SendCommand();
		// then the system will automatically send CCommand_WindowSectionBump commands.
	}
	PeekMessage();
//	CLog::Print("CWindowSection::Reaction( const CWindowSection_CreateWindow& in_rCreateWindow ) end\n");
}

void
CWindowSection::Reaction( long in_SrcSectionID, const CWindowSection_CloseWindow& in_rCloseWindow )
{
	if(m_PreviousHandle)
	{
		CTCommandSender<CWindowSection_CloseWindowResult>::SendCommand(
			in_SrcSectionID,
			CWindowSection_CloseWindowResult(CLWR_PREVIOUS_WINDOW_IS_DIEING,in_rCloseWindow.m_HWnd)
		);
	}
	else if(m_Handle)
	{
		if(in_rCloseWindow.m_HWnd==m_Handle)
		{
			if( DestroyWindow(m_Handle) != 0 )
			{
				m_PreviousHandle = m_Handle;
				m_Handle = 0;
				CTCommandSender<CWindowSection_CloseWindowResult>::SendCommand(
					in_SrcSectionID,
					CWindowSection_CloseWindowResult(CLWR_OK,in_rCloseWindow.m_HWnd)
				);
			}
			else
			{
				CTCommandSender<CWindowSection_CloseWindowResult>::SendCommand(
					in_SrcSectionID,
					CWindowSection_CloseWindowResult(CLWR_ERR,in_rCloseWindow.m_HWnd)
				);
			}
		}
		else
		{
			CTCommandSender<CWindowSection_CloseWindowResult>::SendCommand(
				in_SrcSectionID,
				CWindowSection_CloseWindowResult(CLWR_WRONG_HWND,in_rCloseWindow.m_HWnd)
			);
		}
	}
	else
	{
		CTCommandSender<CWindowSection_CloseWindowResult>::SendCommand(
			in_SrcSectionID,
			CWindowSection_CloseWindowResult(CLWR_NO_WINDOW,in_rCloseWindow.m_HWnd)
		);
	}
	PeekMessage();
}

void
CWindowSection::Reaction( long in_SrcSectionID, const CWindowSection_WindowParams& in_rWindowParams )
{
	assert(false);
	if(m_PreviousHandle)
	{
		CTCommandSender<CWindowSection_WindowParamsResult>::SendCommand(
			in_SrcSectionID,
			CWindowSection_WindowParamsResult(WPR_PREVIOUS_WINDOW_IS_DIEING,in_rWindowParams.m_HWnd)
		);
	}
	else if(m_Handle==NULL)
	{
		CTCommandSender<CWindowSection_WindowParamsResult>::SendCommand(
			in_SrcSectionID,
			CWindowSection_WindowParamsResult(WPR_NO_WINDOW,in_rWindowParams.m_HWnd)
		);
	}
	else if(	(in_rWindowParams.m_HWnd != m_Handle) ||
				(in_rWindowParams.m_ClientAreaWidth<0 || in_rWindowParams.m_ClientAreaHeight<0)
			)
	{
		CTCommandSender<CWindowSection_WindowParamsResult>::SendCommand(
			in_SrcSectionID,
			CWindowSection_WindowParamsResult(WPR_PARAMS,in_rWindowParams.m_HWnd)
		);
	}

	DWORD Style;
	long X,Y;
	long DX,DY;
	GetStylePositionAndDimensions(	// input
									in_rWindowParams.m_WS,
									in_rWindowParams.m_ClientAreaWidth,in_rWindowParams.m_ClientAreaHeight,
									// output
									Style,
									X,Y,
									DX,DY
								);
	bool Success = false;
	do
	{
		DWORD OldStyle = GetWindowLong( m_Handle, GWL_STYLE );
		DWORD NewStyle = ( OldStyle & ~(Style) ) | Style;
		if( 0 == SetWindowLong( m_Handle, GWL_STYLE, NewStyle ) )
			break;
		if( 0 == SetWindowPos( m_Handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED ) )
			break;
		if( 0 == SetWindowPos( m_Handle, HWND_TOP, X, Y, DX, DY, SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOCOPYBITS | SWP_NOSENDCHANGING ) )
			break;
		Success = true;
	} while(false);
	assert(Success);
	CTCommandSender<CWindowSection_WindowParamsResult>::SendCommand(
		in_SrcSectionID,
		CWindowSection_WindowParamsResult( Success?WPR_OK:WPR_ERR, in_rWindowParams.m_HWnd)
	);
	PeekMessage();
}

#define FLAG_ALT_MODIFIER		(1<<29)
#define FLAG_KEY_WAS_RELEASED	(1<<31)
atomic_long in_denial;
void CWindowSection::ProcessMessage(MSG& msg)
{
		if(msg.message==WM_ENTERSIZEMOVE)
		{
			m_LastWidth = -1;
			m_LastHeight = -1;
			m_bResizing = true;
		}
		else if(msg.message==WM_EXITSIZEMOVE)
		{
			CTCommandSender<CWindowSection_WindowSize>::SendCommand(
				SECTION_ID_BROADCAST,
				CWindowSection_WindowSize(false,m_LastWidth,m_LastHeight,m_Handle)
			);
			m_LastWidth = -1;
			m_LastHeight = -1;
			m_bResizing = false;
		}
		else if(msg.message==WM_SIZE)
		{

			if(m_bResizing)
			{
				m_LastWidth =	(long)LOWORD(msg.lParam);
				m_LastHeight =	(long)HIWORD(msg.lParam);
			}
			else
			{
				CTCommandSender<CWindowSection_WindowSize>::SendCommand(
					SECTION_ID_BROADCAST,
					CWindowSection_WindowSize(
						msg.wParam==SIZE_MINIMIZED,
						(long)LOWORD(msg.lParam),
						(long)HIWORD(msg.lParam),
						m_Handle
					)
				);
			}
		}
}
void CWindowSection::IdleProcImpl()
{
	m_pInstance = this;
	MSG msg;
	bool bNewCmdArrived = false;
	in_denial.set(1);
	while( GetMessage(&msg, m_Handle, 0, 0) )
	{
//
// BUMP message
//
		if(msg.message==WM_NEW_COMMAND_ARRIVED)
		{
			in_denial.set(0);
			
			bNewCmdArrived = true;
			break;
		}
		else 
		 ProcessMessage(msg);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

/*	if(!bNewCmdArrived)
	{
		CTCommandSender<CCommand_WindowClosed>::SendCommand(
			SECTION_ID_SYSTEM,
			CCommand_WindowClosed( m_PreviousHandle ? m_PreviousHandle : m_Handle )
		);
		CTCommandSender<CWindowSection_WindowClosed>::SendCommand(
			SECTION_ID_BROADCAST,
			CWindowSection_WindowClosed( m_PreviousHandle ? m_PreviousHandle : m_Handle )
		);
		m_PreviousHandle = NULL;
		m_Handle = NULL;
		m_MagicID = 0;
	}*/

}

void CWindowSection::PeekMessage()
{
	if (m_Handle) 
	{
		MSG msg;
		while( ::PeekMessage(&msg, m_Handle, 0,0, PM_REMOVE) )
		{
	//
	// BUMP message
	//
		
			if(msg.message==WM_NEW_COMMAND_ARRIVED)
			{
				continue;
			}
			else 
				ProcessMessage(msg);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

}


void CWindowSection::KickProcImpl()
{
	if (in_denial.get() ) 
	{
		::PostMessage(m_Handle , WM_NEW_COMMAND_ARRIVED, NULL, NULL);
	}
}



/*void
CWindowSection::Reaction( long in_SrcSectionID, const CCommand_WindowSectionBump& in_rBump )
{
}*/

void
CWindowSection::Reaction( long in_SrcSectionID, const CWindowSection_WindowName& in_rName )
{
	if(m_Handle)
	{
		//m_WindowName = in_rName.m_Name.c_str();
		SetWindowText( m_Handle, in_rName.m_Name.c_str() );
		PeekMessage();
	}
}

void
CWindowSection::Reaction( long in_SrcSectionID, const CWindowSection_TestCommand& in_rTest )
{
	assert(false);
}

void
CWindowSection::GetStylePositionAndDimensions
(	WINDOW_STYLE in_WS,
	long in_ClientWidth, long in_ClientHeight,
	DWORD& out_rStyle,
	long& out_rX, long& out_rY,
	long& out_rDX, long& out_rDY )
{
	assert(in_ClientWidth>0);
	assert(in_ClientHeight>0);
	switch(in_WS)
	{
		case WS_RESIZEABLE: // resizeable window with minimize and maximize options
			out_rStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			break;
		case WS_MINMAX: // non-resizeable window with minimize and maximize options
			out_rStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			break;
		case WS_CONSTANT: // non-resizeable window
			out_rStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
			break;
		case WS_FULLSCREEN:// popup window (for fullscreen rendering)
			out_rStyle = WS_POPUP;
			break;
		default:
			assert(false);
	}
	RECT Rect;
	Rect.left = 0;
	Rect.top = 0;
	Rect.right = in_ClientWidth;
	Rect.bottom = in_ClientHeight;
	AdjustWindowRect( &Rect, out_rStyle, FALSE );
	out_rDX = Rect.right-Rect.left;
	out_rDY = Rect.bottom-Rect.top;
	if(in_WS==WS_FULLSCREEN)
	{
		out_rX = 0;
		out_rY = 0;
	}
	else
	{
		out_rX = ( GetSystemMetrics(SM_CXSCREEN)-out_rDX ) / 2;
		out_rY = ( GetSystemMetrics(SM_CYSCREEN)-out_rDY ) / 2;
	}
	PeekMessage();
}
void CWindowSection::SendQuitMessage()
{
	CTCommandSender<CCommand_WindowClosed>::SendCommand(
					SECTION_ID_SYSTEM,
					CCommand_WindowClosed( m_PreviousHandle ? m_PreviousHandle : m_Handle )
				);
				CTCommandSender<CWindowSection_WindowClosed>::SendCommand(
					SECTION_ID_BROADCAST,
					CWindowSection_WindowClosed( m_PreviousHandle ? m_PreviousHandle : m_Handle )
				);
				m_PreviousHandle = NULL;
				m_Handle = NULL;
				m_MagicID = 0;
}
LRESULT
CALLBACK
CWindowSection::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//m_pInstance->setThread(pCurrentThread);
	switch(message)
	{
		case WM_DESTROY:
			m_pInstance->SendQuitMessage();
			PostQuitMessage(0);
			break;
		case WM_ACTIVATE:
			{
				assert(m_pInstance);
				unsigned short State = LOWORD(wParam);
				bool Minimized = ( HIWORD(wParam) != 0 );
				if(State==WA_INACTIVE)
				{
					CLog::Print("CWindowSection::  sending CWindowSection_Activated\n");
					m_pInstance->SendActivated(false);
				}
				else if( /*!Minimized &&*/ (State==WA_ACTIVE || State==WA_CLICKACTIVE) )
				{
					CLog::Print("CWindowSection::  sending CWindowSection_Activated\n");
					m_pInstance->SendActivated(true);
				}
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void
CWindowSection::SendActivated( bool in_bActive )
{
	CTCommandSender<CWindowSection_Activated>::SendCommand(
		SECTION_ID_BROADCAST,
		CWindowSection_Activated(in_bActive,m_Handle)
	);
}



















