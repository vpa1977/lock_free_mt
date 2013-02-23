#include <windows.h>
#include <assert.h>

const char* g_WindowClassName = "FontWindowClass";

LRESULT
CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			{
				PAINTSTRUCT PaintStruct;
				HDC hdc = BeginPaint(hWnd,&PaintStruct);
				TextOut(hdc, 0, 0, "Hello, World!", 13); 
				EndPaint(hWnd,&PaintStruct);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void
FontRegisterClass(HINSTANCE in_hInstance)
{
	WNDCLASSEX wcex;
	memset( &wcex, 0, sizeof(WNDCLASSEX) );
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= in_hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)COLOR_APPWORKSPACE;
	wcex.hbrBackground	= (HBRUSH)(COLOR_BACKGROUND+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_WindowClassName;
	bool qq = RegisterClassEx(&wcex) != 0;
	assert(qq);
}

int APIENTRY
WinMain
(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	FontRegisterClass(hInstance);
	HWND Handle = CreateWindow(	g_WindowClassName,
									"Fonts test",
									WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
									0,0,
									640,480,
									NULL,
									NULL,
									hInstance,
									NULL
								);
	assert(Handle!=NULL);
	ShowWindow(Handle, nCmdShow);
	UpdateWindow(Handle);

	MSG msg;
	BOOL qq;
	while( qq = GetMessage(&msg, NULL, 0, 0) )
	{
		assert( qq >= 0 );
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}