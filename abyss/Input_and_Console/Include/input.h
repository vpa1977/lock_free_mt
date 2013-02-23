#ifndef INPUT_HPP_INCLUDED
#define INPUT_HPP_INCLUDED

#define STRICT
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <basetsd.h>
#include <dinput.h>

// system



#include "input_console_section_interface.h"
/*

Klass, osushestvlyajushij vsy low-level rabotu s DirectInput-om.

*/

class CInputConsoleSection;


class CInput 
{
public:
	CInput();
	~CInput();

public:
	void SetSection(CInputConsoleSection* parent) { m_pParent = parent;};
	HRESULT InitDirectInput(const CInputConsoleSection_InitInput& initCommand);

	HRESULT InitMouse(HWND hWnd);
	HRESULT InitKeyboard(HWND hWnd);
public:
	void ReadInput(HANDLE h);
	void Acquire();
	void Unaquire();
	void Kick();
	HANDLE GetInputLock() { return m_hInputLock; };

private:
	CInputConsoleSection* m_pParent;


	void ReadMouseInput();
	void ReadKeyboardInput();
	void ReadJoystickInput();


	LPDIRECTINPUT8          m_pDI ;         
	LPDIRECTINPUTDEVICE8    m_pMouse;     
	LPDIRECTINPUTDEVICE8    m_pKeyboard;     
	LPDIRECTINPUTDEVICE8	m_pJoystick;
	bool m_bActive;

	HWND m_hWnd;
	HANDLE m_hInputLock;
	HANDLE m_hMouseInput;
	HANDLE m_hKeyboardInput;
	HANDLE m_hJoystickInput;
	struct {
		long m_dwRelX;
		long m_dwRelY;
		DWORD m_dwX;
		DWORD m_dwY;
		long m_dwZ;
		BOOL m_bLeftPressed;
		BOOL m_bRightPressed;
		BOOL m_bArmed;
		BOOL m_bLeftClick;
	} m_MouseState;
	bool m_bMouseSwapped;

	
};

#endif
