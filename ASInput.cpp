
/*
******************************************************************
* ASInput.cpp
*******************************************************************
* Implements all method prototyped in header
*******************************************************************
*/

#include "ASInput.h"

/*
******************************************************************
* ASInput Constructors and Destructors
*******************************************************************
*/

ASInput::ASInput()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

ASInput::ASInput(const ASInput& inp){}

ASInput::~ASInput(){}

/*
******************************************************************
* Method: Init()
*******************************************************************
* Initialises a new instance of the ASInput class; this 
* initialisation sets all keys to being in a releaed state.
*
* Note: DXInput will be implemented at a later date for better performance
*******************************************************************
*/

bool ASInput::Init(HINSTANCE instance, HWND handle, int width, int height)
{
	HRESULT hr;

	// Set the screen width and height
	m_screenWidth = width;
	m_screenHeight = height;

	// Initialise the mouses current location - updates as user pans view
	m_mouseX = 0;
	m_mouseY = 0;

	// Intiialise the Direct input interface
	hr = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(hr))
		return false;

	// Initialise the keyboard interface
	hr = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(hr))
		return false;

	// Set the data format for the keyboard (Tell DX this device is a keyboard, not a joystick etc...)
	hr = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
		return false;

	// Set keyboard cooperation level to FOREGROUND or EXCLUSIVE, to allow users to be able to use keyboard
	// outside of the game
	hr = m_keyboard->SetCooperativeLevel(handle, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(hr))
		return false;

	// Acquire a lock on the keyboard so the application can use it
	hr = m_keyboard->Acquire();
	if(FAILED(hr))
		return false;

	// Initialise the mouse interface
	hr = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(hr))
		return false;

	// Tell D3D that this is a mouse and not a Joystick etc
	hr = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(hr))
		return false;

	// Set the cooperation level of the mouse 
	hr = m_mouse->SetCooperativeLevel(handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(hr))
		return false;

	// Acquire a lock on the mouse
	hr = m_mouse->Acquire();
	if(FAILED(hr))
		return false;

	return true;
}

/*
******************************************************************
* Method: ProcessFrame
*******************************************************************
* Reads the current state of the input device and sends action to
* process frame, process frame then communicates with ASGraphics
* to draw the new scene
*
* @return bool - true if successfully handled scene, else false
*******************************************************************
*/

bool ASInput::ProcessFrame()
{
	// Check that we can Read updates from mouse and keyboard,
	// and then call to update the scene at ASGraphics.cpp
	if(ReadMouse() && ReadKeyboard())
		HandleInput();
	else 
		return false;

	return true;
}


/*
******************************************************************
* Method: Read keyboard
*******************************************************************
* Reads the state of the keyboard (current keys which are in a down
* state). This can fail if we lose the lock on the keyboard (can 
* happen on window change), if we lose the lock we try to re-acquire
* it, else return false
*
* @return bool - true if successfully read state, else false
*******************************************************************
*/

bool ASInput::ReadKeyboard()
{
	HRESULT hr;

	// Read the keybards state into the the global keyboard state (this will tell us what keys are pressed)
	hr = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);

	// Try to re-acquire a lock to the keyboard, else die
	if(FAILED(hr))
	{
		if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			m_keyboard->Acquire();
		else
			return false;
	}

	// Successfully read the state
	return true;
}


/*
******************************************************************
* Method: Read mouse
*******************************************************************
* Reads the state of the mouse (current pos which are on screen). 
* This can fail if we lose the lock on the mouse (can 
* happen on window change), if we lose the lock we try to re-acquire
* it, else return false
*
* @return bool - true if successfully read state, else false
*******************************************************************
*/

bool ASInput::ReadMouse()
{
	HRESULT hr;

	// Read the mouse state into the global mouse state variable
	hr = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(hr))
	{
		// Try to reacquire the lock on the mouse
		if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			m_mouse->Acquire();
		else 
			return false;
	}

	// Successfully read the state
	return true;
}



/*
******************************************************************
* Method: Handle Input
*******************************************************************
* Handles any input which has came from the keyboard or mouse
*******************************************************************
*/

void ASInput::HandleInput()
{
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure mouse doesnt exit bounds of screen
	if(m_mouseX < 0) m_mouseX = 0;
	if(m_mouseY < 0) m_mouseY = 0;
	if(m_mouseX > m_screenWidth) m_mouseX = m_screenWidth;
	if(m_mouseY > m_screenHeight) m_mouseY = m_screenHeight;

	return;
}



/*
******************************************************************
* Method: Is Escape Down
*******************************************************************
* Checks if the user pressed escape, in the event that they did
* return true to the main system handler
*******************************************************************
*/

bool ASInput::IsEscapeDown()
{
	// Perform a bitwise operation on the keyboard state to check if escape key is down
	// this is used incase multiple keys are down at one time.
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
		return true;

	return false;
}

/*
******************************************************************
* Method: Get Mouse Loc
*******************************************************************
* Gets the current X and Y locations of the mouse on the window
* and writes them to the respective reference parameters
*******************************************************************
*/

void ASInput::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}

/*
******************************************************************
* Method: Release
*******************************************************************
* Dispose of any resources held by ASInput
*******************************************************************
*/

void ASInput::Release()
{
	// Release the lock on the mouse and set its pointer to null
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}
	// Release the lock on the keyboard and set its pointer to null
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}
	// Release the D3D input interface
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
	return;
}