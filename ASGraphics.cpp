
/*
******************************************************************
* ASGraphics.cpp
*******************************************************************
* Implements the prototype methods from ASGraphics.h
*******************************************************************
*/

#include "ASGraphics.h"

/*
*******************************************************************
* Default Constructor
*******************************************************************
* Here we set the default pointer to ASDirect3D to a null pointer,
* this will be assigned in the Init() method, if we didn't assign
* it to a null pointer here, then it could be anything.
*******************************************************************
*/

ASGraphics::ASGraphics() 
{
	m_D3D = 0;
}

/*
*******************************************************************
* Empty Constructor
*******************************************************************
*/

ASGraphics::ASGraphics(const ASGraphics& g)
{}

/*
*******************************************************************
* Destructor
*******************************************************************
* When the object is deleted, we check to see if there is still a
* pointer to it, if this is true then we safely release the object
*******************************************************************
*/

ASGraphics::~ASGraphics()
{
	if(this) { Release(); }
}

/*
*******************************************************************
* Method: Init()
*******************************************************************
* Initalises a new ASGraphics objects
* 
* @param w    : The width of the window (int)
* @param h    : The height of the window (int)
* @param hwnd : The handler of the window (hwnd)
*
* @return bool - True if the window initialised, else false
*******************************************************************
*/

bool ASGraphics::Init(int w, int h, HWND hwnd)
{
	bool success = false;

	// Create a new ASDirect3D object
	m_D3D = new ASDirect3D;
	if(!m_D3D)
		return false;
	
	// Initialise the ASDirect3D object and catch its callback in "success", if 
	// unsuccessful prompt user that the action failed and quit out the application
	success = m_D3D->Init(w, h, SCREEN_DEPTH, SCREEN_NEAR, VSYNC_ENABLED, FULL_SCREEN, hwnd);
	if(!success)
	{
		MessageBox(hwnd, L"Could not Initialise Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

/*
*******************************************************************
* Method: UpdateFrame()
*******************************************************************
* Updates the frame by calling the RenderScene() method for each
* frame - if RenderScene returns false then that causes this
* method to return false to the ASEngine, which in turn will
* terminate the game loop and close the window, safely disposing
* of all objects.
*
* @return bool - True if frame was rendered successfully, else false
*******************************************************************
*/

bool ASGraphics::UpdateFrame()
{
	// Call the render method and catch its return value into "success"
	// to determine if the app should terminate or not
	bool success = RenderScene();
	if(!success)
		return false;
	else 
		return true;
}

/*
*******************************************************************
* Method: RenderScene()
*******************************************************************
* Renders the scene, using the local ASDirect3D delegate object
*
* @return bool - True if the scene rendered successfully, else false
*******************************************************************
*/

bool ASGraphics::RenderScene()
{
	// Clear buffers to begin scene
	m_D3D->PrepareBuffers(0.5f, 0.5f, 0.5f, 1.0f);

	// Present the rendered scene to the screen
	m_D3D->RenderScene();

	return true;
}

/*
*******************************************************************
* Method: Release()
*******************************************************************
* Destroys the ASGraphics instance, safely disposing of any
* resources which it holds onto
* In this case, we release the m_D3D object, delete its reference
* and then set the member pointer to a Null pointer.
*******************************************************************
*/

void ASGraphics::Release()
{
	if(m_D3D)
	{
		m_D3D->Release();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


