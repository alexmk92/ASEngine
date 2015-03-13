
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
	m_lightShader   = 0;
	m_light         = 0;
	m_Camera	    = 0;
	m_Model		    = 0;
	m_D3D		    = 0;
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
*/

ASGraphics::~ASGraphics()
{}

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

	// Initialise a new camera object
	m_Camera = new ASCamera;
	if(!m_Camera)
		return false;

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// Initialise a new model object
	m_Model = new ASModel;
	if(!m_Model)
		return false;

	// Initialise the model, passing the rendering device 
	success = m_Model->Init(m_D3D->GetDevice(), L"./Textures/seafloor.dds");
	if(!success)
	{
		MessageBox(hwnd, L"Error when initialising the model in ASGraphics.cpp, please check ASModel.cpp for errors.", L"Error", MB_OK);
		return false;
	}

	// Initialise a new color shader
	m_lightShader = new ASLightShader;
	if(!m_lightShader)
		return false;

	success = m_lightShader->Init(m_D3D->GetDevice(), hwnd);
	if(!success)
	{
		MessageBox(hwnd, L"Error when initialising the ASTexture object; check the Pixel and Vertex Shader", L"Error", MB_OK);
		return false;
	}

	// Initialise a new ASLight object to illuminate the world!
	m_light = new ASLight;
	if(!m_light)
		return false;

	m_light->SetDirection(0.0f, 0.0f, 1.0f);
	m_light->SetDiffuse(0.8f, 0.0f, 0.8f, 1.0f); // Purple color light 

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
	static float rotation = 0.0f;

	// Update the rotation variable on each frame (used a static var so it doesn't re-init to 0 on each call)
	rotation += (float)D3DX_PI * 0.01f;
	if(rotation > 360.0f)
		rotation -= 360.0f; // reset to 0

	// Call the render method and catch its return value into "success"
	// to determine if the app should terminate or not
	bool success = RenderScene(rotation);
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
* @param rotation - tells us what direction the camera is pointing in
*
* @return bool - True if the scene rendered successfully, else false
*******************************************************************
*/

bool ASGraphics::RenderScene(float rotation)
{
	bool success = false;

	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;

	// Clear the buffers and generate the view matrix for the cameras position
	m_D3D->PrepareBuffers(0.0f, 0.0f, 0.0f, 1.0f);
	m_Camera->RenderCameraView();

	// Retrieve the WVP matrices from the camera
	m_Camera->GetViewMatrix(view);
	m_D3D->GetWorldMatrix(world);
	m_D3D->GetProjectionMatrix(projection);

	// Rotate the world matrix by the rotation matrix 
	D3DXMatrixRotationY(&world, rotation);

	// Send the model to the Graphics pipeline to commence drawing
	m_Model->Render(m_D3D->GetDeviceContext());

	// Render the model using the shader, using the light object to apply lighting to the scene
	success = m_lightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), world, view, projection, m_Model->GetTexture(),
									m_light->GetLightDirection(), m_light->GetDiffuseColor());
	if(!success)
		return false;

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
	// Release the Light Shader
	if(m_lightShader)
	{
		m_lightShader->Release();
		delete m_lightShader;
		m_lightShader = 0;
	}
	// Destroy the Light Object
	if(m_lightShader)
	{
		delete m_light;
		m_light = 0;
	}
	// Release the Model Object
	if(m_Model)
	{
		m_Model->Release();
		delete m_Model;
		m_Model = 0;
	}
	// Release the Camera Object
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	// Release the Rendering Device
	if(m_D3D)
	{
		m_D3D->Release();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


