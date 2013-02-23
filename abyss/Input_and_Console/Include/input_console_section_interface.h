#ifndef INPUT_CONSOLE_SECTION_INTERFACE_HPP_INCLUDED
#define INPUT_CONSOLE_SECTION_INTERFACE_HPP_INCLUDED

#include "../../MT_Core/Include/command.h"
#include <windows.h>






struct CInputConsoleSection_InitInput : public CCommand {
	CInputConsoleSection_InitInput(HWND hwnd, bool needKey, bool needMouse, bool needJoy):
hWnd(hwnd), bNeedKeyboard(needKey),bNeedMouse( needMouse) , bNeedJoystick(needJoy){}
	HWND hWnd;
	bool bNeedKeyboard;
	bool bNeedMouse;
	bool bNeedJoystick;

};
struct CInputConsoleSection_StopInput : public CCommand {
};

struct CInputConsoleSection_MouseUpdate : public CCommand {
	CInputConsoleSection_MouseUpdate() :
		m_dwX(0), m_dwY(0), m_bLeftPressed(false), m_bRightPressed(false), m_dwRelY(0), m_dwRelX(0),
			m_bLeftClick(0)
		{
		}
	long m_dwRelX;
	long m_dwRelY;
	DWORD m_dwX;
	DWORD m_dwY;
	long m_dwZ;
	BOOL m_bLeftPressed;
	BOOL m_bRightPressed;
	BOOL m_bLeftClick;
};

struct CInputConsoleSection_KeyboardUpdate : public CCommand {
	CInputConsoleSection_KeyboardUpdate() : 
m_bPressed(false) , m_bScanCode(0) {}
	BOOL m_bPressed;
	unsigned int m_bScanCode;
};

#define CInputConsoleSection_CLSID 12011966

#endif
