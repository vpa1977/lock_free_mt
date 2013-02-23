#include "Log.h"
#include <windows.h>
#include <assert.h>
#include <stdio.h>

HWND g_hWnd = NULL;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool CreateApplicationWindow(HINSTANCE hInstance, int nCmdShow);

bool bQuitPosted = false;

int APIENTRY
WinMain
(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CLog::Init();
	CLog::Print("begin\n");
	do
	{
		bool qq = CreateApplicationWindow(hInstance,nCmdShow);
		assert(qq);
		if( !qq )
			break;
		MSG msg;
		while( GetMessage(&msg, 0, 0, 0) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while(false);
	CLog::Print("end\n");
	CLog::Close();
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			bQuitPosted = true;
			break;
		case WM_SIZE:
			{
				char Buffer[1024];
				sprintf(Buffer,"WM_SIZE");
				switch(wParam)
				{
					case SIZE_MAXHIDE:
						strcat(Buffer,"  MAXHIDE  ");
						break;
					case SIZE_MAXIMIZED:
						strcat(Buffer,"  MAXIMIZED");
						break;
					case SIZE_MAXSHOW:
						strcat(Buffer,"  MAXSHOW  ");
						break;
					case SIZE_MINIMIZED:
						strcat(Buffer,"  MINIMIZED");
						break;
					case SIZE_RESTORED:
						strcat(Buffer,"  RESTORED ");
						break;
					default:
						break;
				}
				CLog::Print("%s w:%d h:%d\n", Buffer, LOWORD(lParam), HIWORD(lParam) );
			}
			break;
		case WM_SIZING:
			{
				char Buffer[1024];
				sprintf(Buffer,"WM_SIZING");
				switch(wParam)
				{
					case WMSZ_BOTTOM:
						strcat(Buffer," bottom");
						break;
					case WMSZ_BOTTOMLEFT:
						strcat(Buffer," bottom left");
						break;
					case WMSZ_BOTTOMRIGHT:
						strcat(Buffer," bottom right");
						break;
					case WMSZ_LEFT:
						strcat(Buffer," left");
						break;
					case WMSZ_RIGHT:
						strcat(Buffer," right");
						break;
					case WMSZ_TOP:
						strcat(Buffer," top");
						break;
					case WMSZ_TOPLEFT:
						strcat(Buffer," topleft");
						break;
					case WMSZ_TOPRIGHT:
						strcat(Buffer," topright");
						break;
					default:
						break;
				}
				CLog::Print("%s\n", Buffer );
			}
			break;
		case WM_MOVE:
			CLog::Print("WM_MOVE x:%d y:%d\n", (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) );
			break;
		case WM_MOVING:
			CLog::Print("WM_MOVING\n");
			break;
		case WM_ENTERSIZEMOVE:
			CLog::Print("WM_ENTERSIZEMOVE\n");
			break;
		case WM_EXITSIZEMOVE:
			CLog::Print("WM_EXITSIZEMOVE\n");
			break;
		case WM_ACTIVATE:
			{
				char Buffer[1024];
				sprintf(Buffer,"WM_ACTIVATE");
				unsigned short State = LOWORD(wParam);
				switch(State)
				{
					case WA_ACTIVE:
						strcat(Buffer," active");
						break;
					case WA_CLICKACTIVE:
						strcat(Buffer," active_by_click");
						break;
					case WA_INACTIVE:
						strcat(Buffer," inactive");
						break;
					default:
						break;
				}
				unsigned short MinState = HIWORD(wParam);
				CLog::Print("%s%s\n", Buffer, (MinState!=0) ? " minimized":"" );
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

const char* g_WndClassName = "fuckoffanddie";
bool
CreateApplicationWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcex;
	memset( &wcex, 0, sizeof(WNDCLASSEX) );
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_WndClassName;
	bool qq = RegisterClassEx(&wcex) != 0;
	assert(qq);

	g_hWnd = CreateWindow(	g_WndClassName,
							"FUCK",

/* resizeable window */		//WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
/* min-max window */		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
/* non-resizeable window */	//WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,

/* fullscreen window */		//WS_POPUP,

							0,
							0,
							512,
							384,
							NULL,
							NULL,
							hInstance,
							NULL);
	assert(g_hWnd!=NULL);

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return true;
}

