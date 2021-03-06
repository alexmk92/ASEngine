
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
	m_input       = 0;
	m_graphics	  = 0;
	m_environment = 0;
	m_cpuMonitor  = 0;
	m_frameTimer  = 0;
	m_fpsCounter  = 0;
	m_player      = 0;
}

/*
******************************************************************
* ASEngine Empty constructor
******************************************************************
*/

ASEngine::ASEngine(const ASEngine& engine) 
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
		m_input->Init(m_hInstance, m_hwnd, width, height);

	// Initialise a new ASGraphics object, this will be used to render to the screen
	// if a null pointer exists, exit gracefully
	m_graphics = new ASGraphics;
	if(!m_graphics)
		return false;
	else
		success = m_graphics->Init(width, height, m_hwnd);

	// Initalise the sound for the environment 
	/*
	m_environment = new ASSound;
	if(!m_environment)
		return false;
	success = m_environment->Init(m_hwnd);
	if(!success)
	{
		MessageBox(m_hwnd, L"Could not initialise DirectSound", L"Error", MB_OK);
		return false;
	}
	*/
	/*
	* Performance modules - remove these if you don't need to debug or run benchmarks
	*/

	// FPS counter
	m_fpsCounter = new ASFPSCounter;
	if(!m_fpsCounter)
		return false;
	m_fpsCounter->Init();

	// CPU Monitor
	m_cpuMonitor = new ASCPUMonitor;
	if(!m_cpuMonitor)
		return false;
	m_cpuMonitor->Init();

	/*
	* End of performance modules
	*/

	// High Precision Timer - could argue that this is a performance module, however it will be used to
	// synchronise frames
	m_frameTimer = new ASFrameTimer;
	if(!m_frameTimer)
		return false;
	success = m_frameTimer->Init();
	if(!m_frameTimer)
	{
		MessageBox(m_hwnd, L"Could not initialise the Frame Timer.", L"Error", MB_OK);
		return false;
	}

	// Create the player object to traverse the world
	m_player = new ASPlayer;
	if(!m_player)
		return false;

	m_player->SetPosition(SPAWN_X, SPAWN_Y, SPAWN_Z);

	// Catch the value of success, and determine if the window was initalised 
	// without any errors
	if(!success)
		return false;
	else
		return true;

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
		else 
		{
			success = DispatchASEvent();
			if(!success)
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
			}
		}

		// Check if escape was pressed
		if(m_input->IsEscapeDown() == true)
			exit = true;
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
	// Update the input controller with what is being pressed for this current frame
	bool success = m_input->ProcessFrame();
	if(!success)
		return false;

	// On each frame update system statistics
	m_cpuMonitor->UpdateCPUUsage();
	m_fpsCounter->IncrementFrameCount();
	m_frameTimer->GetFrame();

	// Recieve input state from m_Input and do some processing in the scene
	success = ProcessInput(m_frameTimer->GetTime());
	if(!success)
		return false;

	// Update the scene
	success = m_graphics->RenderScene(m_camInfo);
	if(!success)
		return false;

	return true;
}

/*
******************************************************************
* Method: Process Input
******************************************************************
* Private interface to recieve the input from the ASInput controller
* and then do some processing to update the scene
*
* @param float - the frame time in FPS so all frames are synchronised
* @return bool : True if successful callback, else False
******************************************************************
*/

bool ASEngine::ProcessInput(float frameTime)
{
	// Bool to determine if the key is down (defaults to false and will be set 
	// by querying the D3D input controller)
	bool isKeyDown;
	bool success;

	m_player->SetFrameTime(frameTime);

	// Handle input
	isKeyDown = m_input->IsLeftArrowDown();
	m_player->TurnLeft(isKeyDown);

	isKeyDown = m_input->IsRightArrowDown();
	m_player->TurnRight(isKeyDown);

	isKeyDown = m_input->IsUpArrowDown();
	m_player->MoveForward(isKeyDown);

	isKeyDown = m_input->IsDownArrowDown();
	m_player->MoveBackward(isKeyDown);

	isKeyDown = m_input->IsSpaceBarDown();
	m_player->MoveUpward(isKeyDown);

	// Set the Camera info data structure
	m_player->GetPosition(m_camInfo.pos.x, m_camInfo.pos.y, m_camInfo.pos.z);
	m_player->GetRotation(m_camInfo.rot.x, m_camInfo.rot.y, m_camInfo.rot.z);

	// Everything was successful
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
		width  = 1280;
		height = 720;

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
	return DefWindowProc(hwnd, umsg, wparam, lparam);
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
	// Release any sound object
	if(m_environment)
	{
		m_environment->Release();
		delete m_environment;
		m_environment = 0;
	}
	// Release the input object, jump straight to step 2
	// as m_input does not allocate any new resources
	if(m_input)
	{
		m_input->Release();
		delete m_input;
		m_input = 0;
	}
	// Release the performance objects
	if(m_frameTimer)
	{
		delete m_frameTimer;
		m_frameTimer = 0;
	}
	// Release the cpu monitor
	if(m_cpuMonitor)
	{
		m_cpuMonitor->Release();
		delete m_cpuMonitor;
		m_cpuMonitor = 0;
	}
	// Release the fps counter
	if(m_fpsCounter)
	{
		delete m_fpsCounter;
		m_fpsCounter = 0;
	}
	// release the player object
	if(m_player)
	{
		m_player->Release();
		delete m_player;
		m_player = 0;
	}

	// Close the Window
	CloseASWindow();

	return;
}