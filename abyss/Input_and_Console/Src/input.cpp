#include "../Include/input.h"
#include "../Include/input_console_section_interface.h"
#include "../Include/input_console_section.h"
#include "../../MT_Core/Include/log.h"


CInput::CInput()
:
m_pDI(0),         
m_pMouse(0),     
m_pKeyboard(0),     
m_pJoystick(0),
m_bActive(false), 
m_pParent(NULL)
{
	m_MouseState.m_bLeftPressed = false;
	m_MouseState.m_bRightPressed = false;
	m_MouseState.m_bArmed = false;
	m_MouseState.m_bLeftClick = false;

	m_MouseState.m_dwRelX =0;
	m_MouseState.m_dwRelY =0;
	m_MouseState.m_dwX =0;
	m_MouseState.m_dwY = 0;
}

CInput::~CInput()
{

}

void CInput::Kick()
{
	SetEvent(m_hInputLock);
}

void CInput::Unaquire()
{
	if (!m_pMouse)
		return;
	HRESULT hr= S_OK;
	hr = m_pMouse->Unacquire();
	//	m_MouseState.m_bArmed = false;
	//	m_MouseState.m_bLeftClick = false;

	hr = m_pKeyboard->Unacquire();
	if (FAILED(hr)) {
		CLog::Println("Failed to unacquire device");
		switch (hr) {
			case DIERR_INVALIDPARAM :
				{
					CLog::Println("Invalid parameter");
					
				}break;
			case DIERR_NOTINITIALIZED: 
				{
					CLog::Println("Not initialized");
					
				}break;
			case DIERR_OTHERAPPHASPRIO : 
				{
					CLog::Println("Other app priority");
					
				}break;


		}
	}
}
void CInput::Acquire()
{

	if (!m_pMouse)
		return;
	HRESULT hr= S_OK;
	m_bActive = true;
	if (m_bActive) 
	{
		hr = m_pMouse->Acquire();
	//	m_MouseState.m_bArmed = false;
	//	m_MouseState.m_bLeftClick = false;

		if (!FAILED(hr))
		{
			hr = m_pKeyboard->Acquire();
		}
	}
	else
	{
	//	hr = m_pMouse->Unacquire();
	//	if (!FAILED(hr))
	//	{
	//		hr = m_pKeyboard->Unacquire();
	//	}

	}

	if (FAILED(hr)) {
		CLog::Println("Failed to acquire device");
		switch (hr) {
			case DIERR_INVALIDPARAM :
				{
					CLog::Println("Invalid parameter");
					
				}break;
			case DIERR_NOTINITIALIZED: 
				{
					CLog::Println("Not initialized");
					
				}break;
			case DIERR_OTHERAPPHASPRIO : 
				{
					CLog::Println("Other app priority");
					
				}break;


		}
	}
}


HRESULT CInput::InitDirectInput(const CInputConsoleSection_InitInput& initCommand)
{
    HRESULT hr = 0;

/*	m_hInputLock = CreateEvent( NULL, FALSE, FALSE, NULL );
	if (!m_hInputLock) 
	{
		return E_FAIL;
	}*/
	
	m_hMouseInput = CreateEvent( NULL, FALSE, FALSE, NULL );
	if (!m_hMouseInput) 
	{
		return E_FAIL;
	}

	m_hKeyboardInput = CreateEvent( NULL, FALSE, FALSE, NULL );
	if (!m_hKeyboardInput) 
	{
		return E_FAIL;
	}

	m_hJoystickInput = CreateEvent( NULL, FALSE, FALSE, NULL );
	if (!m_hJoystickInput) 
	{
		return E_FAIL;
	}



    hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ;
	
	if (initCommand.bNeedMouse)
	if (FAILED(hr=InitMouse(initCommand.hWnd)))
           return hr;

	if (initCommand.bNeedKeyboard)
	if (FAILED(hr=InitKeyboard(initCommand.hWnd)))
           return hr;

	m_hWnd = initCommand.hWnd;
	return hr;
}

HRESULT CInput::InitKeyboard(HWND hWnd)
{
	HRESULT hr=0;
	    
    if( FAILED( hr = m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL ) ) )
        return hr;

    
    if( FAILED( hr = m_pKeyboard->SetDataFormat( &c_dfDIKeyboard  ) ) )
        return hr;

    if( FAILED( hr = m_pKeyboard->SetCooperativeLevel( hWnd, 
                                         DISCL_EXCLUSIVE| DISCL_FOREGROUND ) ) )
	 return hr;

	if (FAILED(hr=m_pKeyboard->SetEventNotification(m_hKeyboardInput)))
		return hr;
	
	DIPROPDWORD  dipdw; 

	dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	dipdw.diph.dwObj = 0; 
	dipdw.diph.dwHow = DIPH_DEVICE; 
	dipdw.dwData = 10; 
	if (FAILED(hr = m_pKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
	{
		return hr;
	}
	if (FAILED(m_pKeyboard->Acquire()))
	{
		return hr;
	}


	return hr;
}

HRESULT CInput::InitMouse(HWND hWnd)
{
	HRESULT hr=0;
	 
    m_bMouseSwapped  = GetSystemMetrics( SM_SWAPBUTTON );

    if( FAILED( hr = m_pDI->CreateDevice( GUID_SysMouse, &m_pMouse, NULL ) ) )
        return hr;

    
    if( FAILED( hr = m_pMouse->SetDataFormat( &c_dfDIMouse ) ) )
        return hr;

    if( FAILED( hr = m_pMouse->SetCooperativeLevel( hWnd, 
					DISCL_NONEXCLUSIVE| DISCL_FOREGROUND  ) ) )
	 return hr;

	if (FAILED(hr=m_pMouse->SetEventNotification(m_hMouseInput)))
		return hr;

	// set buffer size to hold 10 entries
	DIPROPDWORD  dipdw; 

	dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	dipdw.diph.dwObj = 0; 
	dipdw.diph.dwHow = DIPH_DEVICE; 
	dipdw.dwData = 10; 
	if (FAILED(hr = m_pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
	{
		return hr;
	}



	if (FAILED(m_pMouse->Acquire()))
	{
		return hr;
	}


	

	return hr;
}


void CInput::ReadInput(HANDLE h)
{
	m_hInputLock = h;
	HANDLE ah[4] = { m_hInputLock , m_hMouseInput , m_hKeyboardInput, m_hJoystickInput };
//	Acquire();
	DWORD dwResult = WaitForMultipleObjects(4, ah, FALSE, 
                       INFINITE); 
	//CLog::Println("Input lock broken");
    switch (dwResult) { 
    case WAIT_OBJECT_0: 
        // Input Lock is broken . Exit method
		{
			CLog::Println("Broken Lock");
			ResetEvent(m_hInputLock);
		
		}
        break; 
 
    case WAIT_OBJECT_0 + 1: 
        ReadMouseInput(); // read mouse input
        break; 
 
    case WAIT_OBJECT_0 + 2: 
        ReadKeyboardInput(); 
        break; 

	case WAIT_OBJECT_0 + 3: 
        ReadJoystickInput(); 
        break; 
    default: 
		// error
		CLog::Println("Event Error ");
        break; 
    } 
 
}


void CInput::ReadMouseInput()
{
    BOOL                bDone;
    DIDEVICEOBJECTDATA  od;
    DWORD               dwElements;
    HRESULT             hr;

    // Attempt to read one data element.  Continue as long as
    // device data is available.
    bDone = FALSE;
	long dwSequence = 0;
    while( true ) 
    {
        dwElements = 1;
        hr = m_pMouse->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), 
                                      &od, &dwElements, 0 );

        if( hr == DIERR_INPUTLOST ) 
        {
            m_pMouse->Acquire();
			m_MouseState.m_bLeftPressed = false;
			m_MouseState.m_bRightPressed = false;
			m_MouseState.m_bArmed = false;
			m_MouseState.m_bLeftClick = false;
			m_MouseState.m_dwX = 0;
			m_MouseState.m_dwY = 0;
			m_MouseState.m_dwRelX = 0;
			m_MouseState.m_dwZ = 0;
			m_MouseState.m_dwRelY = 0;
            continue;
        }

        // Unable to read data or no data available
        if( FAILED(hr) || dwElements == 0 ) 
        {
			if (bDone){
				CInputConsoleSection_MouseUpdate update;
				update.m_bLeftPressed = m_MouseState.m_bLeftPressed;
				update.m_bRightPressed = m_MouseState.m_bRightPressed;
				update.m_dwX = m_MouseState.m_dwX;
				update.m_dwY = m_MouseState.m_dwY;
				update.m_dwZ = m_MouseState.m_dwZ;
				update.m_dwRelX  = m_MouseState.m_dwRelX;
				update.m_dwRelY  = m_MouseState.m_dwRelY;
				update.m_bLeftClick = m_MouseState.m_bLeftClick;
				
				m_pParent->SendMouseUpdate(update);
				m_MouseState.m_bLeftClick = false;
		
			}
           return;
		}
		
		// push event.
		if (bDone && dwSequence != od.dwSequence ) 
		{
			CInputConsoleSection_MouseUpdate update;
			update.m_bLeftPressed = m_MouseState.m_bLeftPressed;
			update.m_bRightPressed = m_MouseState.m_bRightPressed;
			update.m_dwX = m_MouseState.m_dwX;
			update.m_dwY = m_MouseState.m_dwY;
			update.m_dwZ = m_MouseState.m_dwZ;
			update.m_dwRelX  = m_MouseState.m_dwRelX;
			update.m_dwRelY  = m_MouseState.m_dwRelY;
			update.m_bLeftClick = m_MouseState.m_bLeftClick;
			
			m_pParent->SendMouseUpdate(update);
		
			m_MouseState.m_bLeftClick = false;

		}

		// next event
		dwSequence = od.dwSequence;

        bDone = true;
		
        // Look at the element to see what happened
        switch( od.dwOfs ) 
        {     
            case DIMOFS_X:       // Mouse horizontal motion 
				{
					POINT p;
					m_MouseState.m_dwRelX = od.dwData;
					m_MouseState.m_dwRelY = 0;
					::GetCursorPos(&p);
					::ScreenToClient( m_hWnd, &p );
					m_MouseState.m_dwX = p.x;
					m_MouseState.m_dwY = p.y;
					m_MouseState.m_dwZ =0;
				}
                 break;

            case DIMOFS_Y:       // Mouse vertical motion 
				{
					POINT p;
					m_MouseState.m_dwRelX = 0;
					m_MouseState.m_dwRelY = od.dwData;
					::GetCursorPos(&p);
					::ScreenToClient( m_hWnd, &p );
					m_MouseState.m_dwX = p.x;
					m_MouseState.m_dwY = p.y;
					m_MouseState.m_dwZ =0;
				}

                break;
			case DIMOFS_Z:
				{
					m_MouseState.m_dwZ = od.dwData;
				}	
				break;

            case DIMOFS_BUTTON0: // Right button pressed or released 
				{
					if (m_bMouseSwapped)
					{
						if (od.dwData & 0x80 ) 
						{
							m_MouseState.m_bRightPressed = TRUE;
						}
						else
						{
							m_MouseState.m_bRightPressed = FALSE;
						}
					}
					else
					{

						if (od.dwData & 0x80 ) 
						{
							m_MouseState.m_bLeftPressed = TRUE;
						}
						else
						{
							m_MouseState.m_bLeftPressed = FALSE;
						}
					
					}
					break;                    
				}
            case DIMOFS_BUTTON1: // Left button pressed or released 
                // Is the right or a swapped left button down?
				{
					if (m_bMouseSwapped)
					{
						if (od.dwData & 0x80 ) 
						{
							m_MouseState.m_bLeftPressed = TRUE;
						}
						else
						{
							m_MouseState.m_bLeftPressed = FALSE;
						}
					}
					else
					{
						if (od.dwData & 0x80 ) 
						{
							m_MouseState.m_bRightPressed = TRUE;
						}
						else
						{
							m_MouseState.m_bRightPressed = FALSE;
						}

					}
					break;
				}
        } // end switch
		
		if (m_MouseState.m_bLeftPressed && !m_MouseState.m_bArmed) 
		{
			m_MouseState.m_bArmed = true;
			m_MouseState.m_bLeftClick = true;
		}
		if (!m_MouseState.m_bLeftPressed && m_MouseState.m_bArmed) 
		{
			
			m_MouseState.m_bArmed = false;
		}

		
    }
	

}

void CInput::ReadKeyboardInput()
{
    BOOL                bDone;
    DIDEVICEOBJECTDATA  od;
    DWORD               dwElements;
    HRESULT             hr;

	bDone = FALSE;
	CInputConsoleSection_KeyboardUpdate update;
    while( !bDone ) 
    {
        dwElements = 1;
        hr = m_pKeyboard->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), 
                                      &od, &dwElements, 0 );

        if( hr == DIERR_INPUTLOST ) 
        {
            m_pKeyboard->Acquire();
            continue;
        }

		if (hr == DIERR_NOTBUFFERED )
		{
			return;
		}

		if (hr == DIERR_INVALIDPARAM) 
		{
			return;
		}

		if (hr == DIERR_NOTACQUIRED) 
		{
			return;
		}

        // Unable to read data or no data available
        if( FAILED(hr) || dwElements == 0 ) 
        {
            return;
        }
        
      
	   update.m_bPressed = od.dwData & 0x80;
	   update.m_bScanCode = od.dwOfs;
		m_pParent->SendKeyboardUpdate(update);	
    }
	
	
}

void CInput::ReadJoystickInput()
{
}

