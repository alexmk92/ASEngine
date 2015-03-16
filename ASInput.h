
/*
******************************************************************
* ASInput.h
*******************************************************************
* Handles all I/O events triggered by the user, method calls in
* this class are recieved via ASEngine.h, this class is essentially
* a delegate of ASEngine
*******************************************************************
*/

#ifndef _ASINPUT_H_
#define _ASINPUT_H_

// Pre-processor symbols
#define DIRECTINPUT_VERSION 0x0800

// Link to external DX libraries required for DX input
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// Inlcudes
#include <dinput.h>

class ASInput 
{
public:
	// Constructors and Destructor prototypes
	ASInput();
	ASInput(const ASInput&);
	~ASInput();

	// Initialise method
	bool Init(HINSTANCE, HWND, int, int);
	void Release();
	bool ProcessFrame();
	bool IsEscapeDown();
	// Left, Right, Up, Down controls
	bool IsLeftArrowDown();
	bool IsRightArrowDown();
	bool IsUpArrowDown();
	bool IsDownArrowDown();
	bool IsSpaceBarDown();
	// Mouse panning
	bool LeftMouseClicked();
	bool RightMouseClicked();

	// Returns the X, Y coords of the mouse
	void GetMouseLocation(int&, int&);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void HandleInput();

	// Member variables
	IDirectInput8*		 m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	unsigned char		 m_keyboardState[256];	// holds current keys down
	DIMOUSESTATE		 m_mouseState;			// holds current mouse press
	int					 m_screenWidth;			// screen dimens
	int					 m_screenHeight;
	int					 m_mouseX;				// mouse pos rel to HWND
	int				     m_mouseY;
	bool				 m_mouseDown;
};

#endif