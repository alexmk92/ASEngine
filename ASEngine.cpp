
/*
******************************************************************
* ASEngine.cpp
*******************************************************************
* Implements the prototype methods from ASEngine.h
*******************************************************************
*/

#include "ASEngine.h"

/*
******************************************************************
* Default constructor - sets input and graphic pointers to null
* in the event that the Release function is called on a invalid
* initialise, this will clean the program up gracefully
******************************************************************
*/

ASEngine::ASEngine()
{
	m_input    = 0;
	m_graphics = 0;
}

/*
******************************************************************
* ASEngine Empty constructor
******************************************************************
*/

ASEngine::ASEngine(const ASEngine& e) 
{}

/*
******************************************************************
* ASEngine destructor 
******************************************************************
*/

ASEngine::~ASEngine()
{}

/*
******************************************************************
* Method: Init
******************************************************************
* Initalises a new instance of the ASEngine class and sets up
* a new window as well as I/O class pointers to allow user input
* and the continued interaction of drawing graphics to the canvas
*
* @return - bool : True if successful initialisation, else False
******************************************************************
*/

bool ASEngine::Init() 
{
	// Declare and initialise local variables
	int  width   = 0;
	int  height  = 0;
	bool success = false;

	// Initialise the WinAPI to build window
	InitASWindow(width, height);

	// Initialise a new ASInput object, this will be used to handle keyboard and
	// mouse I/O operations, if a null pointer exists exit gracefully
	m_input = new ASInput;
	if(!m_input)
		return false;
	else
		m_input->Init();

	// Initialise a new ASGraphics object, this will be used to render to the screen
	// if a null pointer exists, exit gracefully
	m_graphics = new ASGraphics;
	if(!m_graphics)
		return false;
	else
		success = m_graphics->Init(width, height, m_hwnd);

	// Catch the value of success, and determine if the window was initalised 
	// without any errors
	if(!success)
		return false;
	else
		return true;

}

/*
******************************************************************
* Method: Release()
******************************************************************
* Disposes of any resources held onto by the ASEngine class.
* In order to ensure cleanup is handled with no error we:
*   1) Call the release method of the object to dispose of its resources
*   2) delete the pointer to the object
*   3) Set the pointer to 0, so we safely know its a null pointer
* This ensures everything is safely de-allocated from memory
******************************************************************
*/

void ASEngine::Release()
{
	// Release the graphics object
	if(m_graphics)
	{
		m_graphics->Release();
		delete m_graphics;
		m_graphics = 0;
	}

	// Release the input object, jump straight to step 2
	// as m_input does not allocate any new resources
	if(m_input)
	{
		delete m_input;
		m_input = 0;
	}

	// Close the Window
	CloseASWindow();

	return;
}

/*
******************************************************************
* Method: Run
******************************************************************
* Starts the main game loop, this will listen for events and 
* dispatch system messages to be handled to do process X in the
* game
******************************************************************
*/

void ASEngine::Run()
{
	MSG  m;
	bool exit    = false;
	bool success = false;

	// Initialise the message structure - this sets all bytes in the struct to 0
	ZeroMemory(&m, sizeof(MSG));

	// Game loop - keep listening on the message stack, every time a new
	// message is encountered process it, if "exit" evals to true then
	// gracefully exit the program by calling the Release() method
	while(!exit)
	{
		if(PeekMessage(&m, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&m);
			DispatchMessage(&m);
		}

		// Check if an exit message was sent on the stack - if so set exit to true,
		// otherwise dispatch the message to DispatchASEvent() to handle rendering, then 
		// check if the Frame was processed successfully, if not exit the application
		if(m.message == WM_QUIT)
			exit = true;
		else {
			success = DispatchASEvent();
			if(!success)
				exit = true;
		}
	}

	return;
}

/*
******************************************************************
* Method: DispatchASEvent
******************************************************************
* Recieves a message from the processing queue and dispatches
* it to the ASGraphic object; ASGraphics will then handle the
* update of the scene and return a bool based on its success
*
* @return - bool : True if successful callback, else False
******************************************************************
*/

bool ASEngine::DispatchASEvent() 
{
	// Flag to determine state
	bool success = false;

	// Check if the user wanted to exit the program
	if(m_input->IsKeyDown(VK_ESCAPE))
		return false;

	// Dispatch the event to the graphics object
	success = m_graphics->UpdateFrame();
	if(!success)
		return false;
	else
		return true;
}

/*
******************************************************************
* Method: InitASWindow
******************************************************************
* Intialises a new WinAPI window object to a given width and height
* the window will either be FULL_SCREEN or a default size, dependent
* on system configuration variables.
*
* @param width  - The width of the window  (int)
* @param height - The height of the window (int)
******************************************************************
*/

void ASEngine::InitASWindow(int& width, int& height)
{
	// Local variables
	WNDCLASSEX window;
	DEVMODE    scrSettings;

	// Default anchor screen to top left of screen
	int        posX = 0;
	int        posY = 0;

	// Set the global Engine instance handle (defined ASEngine.h), 
	// we now only ever have ASEngine object in memory
	EngineInstance = this;

	// Allocate member variables of ASEngine 
	m_hInstance = GetModuleHandle(NULL);
	m_appName   = L"ASEngine";

	// Initialise the window via WinAPI methods
	window.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window.lpfnWndProc   = WndProc;
	window.cbClsExtra    = 0;
	window.cbWndExtra    = 0;
	window.hInstance     = m_hInstance;
	window.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	window.hIconSm       = window.hIcon;
	window.hCursor       = LoadCursor(NULL, IDC_ARROW);
	window.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	window.lpszMenuName  = NULL;
	window.lpszClassName = m_appName;
	window.cbSize        = sizeof(WNDCLASSEX);

	// Register the window
	RegisterClassEx(&window);

	// Set width and height dependent on client screen
	width  = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	// Determine if the screen is FullScreen or not, and then draw the instance to the screen
	if(FULL_SCREEN)
	{
		// Set the screen to the maximum size of users desktop
		memset(&scrSettings, 0, sizeof(scrSettings));
		scrSettings.dmSize = sizeof(scrSettings);
		scrSettings.dmPelsWidth = (unsigned long)width;
		scrSettings.dmPelsHeight = (unsigned long)height;
		scrSettings.dmBitsPerPel = 32;
		scrSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Update the display via WinAPI
		ChangeDisplaySettings(&scrSettings, CDS_FULLSCREEN);
	} 
	else 
	{
		width  = 800;
		height = 600;

		// Place window in middle of screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	}

	// Create the window with the settings defined above and get its handle 
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_appName, m_appName, 
						    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, 
							posX, posY, width, height, NULL, NULL, m_hInstance, NULL);

	// Display the window on screen by using the handle we recieved from CreateWindowEx callback
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	ShowCursor(false);

	return;
}

/*
******************************************************************
* Method: CloseASWindow
******************************************************************
* Disposes of all resources held by the ASWindow gracefully
******************************************************************
*/

void ASEngine::CloseASWindow()
{
	// If cursor was disabled, re-show it
	ShowCursor(true);

	// If the application was in full-screen mode, restore user
	// defaults, otherwise nasty error will occur
	if(FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	// Remove the window from screen using its handle
	DestroyWindow(m_hwnd);
	m_hwnd = 0;

	// Remove application instance
	UnregisterClass(m_appName, m_hInstance);
	m_hInstance = 0;

	// Release pointer to the class (as we've destroyed it)
	EngineInstance = 0;

	return;
}

/*
******************************************************************
* Method: MessageHandler
******************************************************************
* Implementation of the WinAPI MessageHandler function, this
* method determines what input the user sending (keypress, mouseclick)
* and then fires an appropriate call to the input controller to set
* the current state
******************************************************************
*/

LRESULT CALLBACK ASEngine::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		// One or more keys on the keyboard are in a down state
		case WM_KEYDOWN:
		{
			m_input->KeyDown((unsigned int)wparam);
			return 0;
		}
		// No key on the keyboard is currently in a down state
		case WM_KEYUP:
		{
			m_input->KeyUp((unsigned int)wparam);
			return 0;
		}
		// Left mouse button is being held down
		case WM_LBUTTONDOWN:
		{
			m_input->LeftMBDown();
			return 0;
		}
		// Left mouse button has been released
		case WM_LBUTTONUP:
		{
			m_input->LeftMBUp();
			return 0;
		}
		// Default message handler
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

/*
******************************************************************
* Method: WndProc
******************************************************************
* Implementation of the WinAPI method WndProc, this is where
* Windows sends all messages recieved by the user in Window X
* This method is used by MessageHandler(), MessageHandler will
* only be called if WM_DESTROY or WM_CLOSE events are not called,
* this improves the efficency of message routing in the application
******************************************************************
*/

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is to be destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		// Check if the window is to be closed
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		// Other messages are sent to the MessageHandler class (I/O events)
		default:
		{
			return EngineInstance->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}