
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
	m_Text			= 0;
	m_Frustum       = 0;
	m_EnemyList     = 0;
	m_WorldTerrain  = 0;
	m_colorShader   = 0;
	m_terrainShader = 0;
	m_quadTree      = 0;
	m_skyShader     = 0;
	m_skyBox        = 0;
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
using namespace std;
bool ASGraphics::Init(int w, int h, HWND hwnd)
{
	bool success = false;
	D3DXMATRIX viewMatrix;

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

	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->RenderCameraView();
	m_Camera->GetViewMatrix(viewMatrix);

	// Set the spawn location of the player
	m_Camera->SetPosition(SPAWN_X, SPAWN_Y, SPAWN_Z);

	// Initialise the new terrain object
	m_WorldTerrain = new ASTerrain;
	if(!m_WorldTerrain)
		return false;

	// Build an array of textures to pass to the terrain
	vector<WCHAR*> textures;
	textures.push_back(L"./textures/grass.dds");
	textures.push_back(L"./textures/rock.dds");
	textures.push_back(L"./textures/slope.dds");
	success = m_WorldTerrain->Init(m_D3D->GetDevice(), "./textures/mapC.bmp", "./textures/colorMap.bmp", textures, L"./textures/detail.dds");
	if(!success) {
		MessageBox(hwnd, L"Error when initialising the world terrain in ASGraphics.cpp.", L"Error", MB_OK);
		return false;
	}

	// Initialise a new model object
	m_Model = new ASModel;
	if(!m_Model)
		return false;

	// Initialise the model, passing the rendering device 
	/*
	success = m_Model->Init(m_D3D->GetDevice(), L"./textures/seafloor.dds", "./models/cube.txt");
	if(!success)
	{
		MessageBox(hwnd, L"Error when initialising the model in ASGraphics.cpp, please check ASModel.cpp for errors.", L"Error", MB_OK);
		return false;
	}*/

	// Initiase a new terrain shader
	m_terrainShader = new ASTerrainShader;
	if(!m_terrainShader)
		return false;

	success = m_terrainShader->Init(m_D3D->GetDevice(), hwnd);
	if(!success)
	{
		MessageBox(hwnd, L"Error when initialisng the ASTerrainshader.", L"Error", MB_OK);
		return false;
	}

	// Build the enemies into the world
	m_EnemyList = new ASEnemies;
	if(!m_EnemyList)
		return false;

	// Initialise the model list
	success = m_EnemyList->Init(30);
	if(!success) {
		MessageBox(hwnd, L"Could not initialise the enemy list.", L"Error", MB_OK);
		return false;
	}

	// Initialise a new ASLight object to illuminate the world!
	m_light = new ASLight;
	if(!m_light)
		return false;

	m_light->SetAmbient(0.45f, 0.45f, 0.45f, 1.0f);
	m_light->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(-0.5f, -1.0f, 0.0f);

	// Create frustrum
	m_Frustum = new ASFrustrum;
	if(!m_Frustum)
		return false;

	// Create the quad tree
	m_quadTree = new ASQuadTree;
	if(!m_quadTree)
		return false;
	success = m_quadTree->Init(m_D3D->GetDevice(), m_WorldTerrain);
	if(!success)
	{
		MessageBox(hwnd, L"Could not initialise the quad tree", L"Error", MB_OK);
		return false;
	}

	// Create the new sky box
	m_skyBox = new ASSkyBox;
	if(!m_skyBox)
		return false;

	success = m_skyBox->Init(m_D3D->GetDevice());
	if(!success)
	{
		MessageBox(hwnd, L"Error: Could not initialise the sky box", L"Error", MB_OK);
		return false;
	}

	// Create the skybox shader
	m_skyShader = new ASSkyShader;
	if(!m_skyShader)
		return false;

	success = m_skyShader->Init(m_D3D->GetDevice(), hwnd, L"./ASSkyBox.vs", L"./ASSkyBox.ps");
	if(!success) {
		MessageBox(hwnd, L"Error: Could not initialise the sky shader, it's likely an invalid file path was passed.", L"Error", MB_OK);
		return false;
	}

	/*
	// Create the text object.
	m_Text = new ASText;
	if(!m_Text)
		return false;

	// Initialize the text object.
	success = m_Text->Init(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, w, h, viewMatrix);
	if(!success)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}
	*/

	return true;
}

/*
*******************************************************************
* Method: RenderScene()
*******************************************************************
* Updates each frame if RenderScene returns false then that causes this
* method to return false to the ASEngine, which in turn will
* terminate the game loop and close the window, safely disposing
* of all objects.
* Renders the scene, using the local ASDirect3D delegate object
* 
* @param ASCameraInfo - the current camera infomation
*
* @return bool - True if the scene rendered successfully, else false
*******************************************************************
*/

bool ASGraphics::RenderScene(ASCameraInfo info)
{
	bool success	 = false;
	bool renderModel = false;

	// Used for frustum culling
	int numEnemies = 0;
	int renderCount = 0;

	float camHeight = 0;
	D3DXVECTOR4 color;
	D3DXVECTOR3 camPos;
	D3DXVECTOR3 camRot;

	// WVP Matrices
	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;
	D3DXMATRIX ortho;
	D3DXMATRIX camWorld;

	// Retrieve the WVP matrices from the camera to perform any translations
	m_D3D->GetWorldMatrix(world);
	m_Camera->GetViewMatrix(view);
	m_D3D->GetProjectionMatrix(projection);
	m_D3D->GetInterfaceMatrix(ortho);
	m_Camera->GetWorldMatrix(camWorld);

	// Clear the back buffer and prepare to draw the scene
	m_D3D->PrepareBuffers(0.0f, 0.0f, 0.0f, 1.0f);

	// Set the new position of the camera, then return the new set values into local vars
	m_Camera->SetPosition(info.pos.x, info.pos.y, info.pos.z);
	m_Camera->SetRotation(info.rot.x, info.rot.y, info.rot.z);
	camPos = m_Camera->GetPosition();
	camRot = m_Camera->GetRotation();

	// Get the cameras current pos then get set the cameras new position based on the height of the
	// triangle that is directly underneath
	if(m_quadTree->GetTerrainHeightAtPosition(camPos.x, camPos.z, camHeight))
											  m_Camera->SetPosition(camPos.x, camHeight + 2.5f, camPos.z);

	// Prepare the sky (this should be drawn first as backface culling must be disabled to ensure
	// that the sky is not culled (excluded) from the view.)
	// Create a translation matrix to move the sky relative to the viewer
	D3DXMatrixTranslation(&world, info.pos.x, info.pos.y + camHeight + 0.25, info.pos.z);

	m_D3D->DisableCulling();
	m_D3D->DisableZBuffer();

	m_skyBox->Render(m_D3D->GetDeviceContext());
	m_skyShader->RenderSky(m_D3D->GetDeviceContext(), m_skyBox->GetNumIndices(), world, view, projection, m_skyBox->GetColorA(), m_skyBox->GetColorB());

	m_D3D->EnableZBuffer();
	m_D3D->EnableCulling();

	// Reset the world matrix for the rest of the components
	m_D3D->GetWorldMatrix(world);

	// Clear the buffers and generate the view matrix for the cameras position
	m_Camera->RenderCameraView();

	// build the frustum
	m_Frustum->ConstructFrustrum(SCREEN_DEPTH, projection, view);


	// Build the terrain
	vector<ID3D11ShaderResourceView*> res;
	m_WorldTerrain->GetTextures(res);
	success = m_terrainShader->SetShaderParameters(m_D3D->GetDeviceContext(), world, view, projection, m_light->GetAmbientColor(), 
												   m_light->GetDiffuseColor(), m_light->GetLightDirection(), m_WorldTerrain->GetDetailTexture(), res);

	if(!success)
		return false;

	// Render the terrain using the quad tree renderer
	m_quadTree->Render(m_Frustum, m_terrainShader, m_D3D->GetDeviceContext());

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
	// Release the text object
	if(m_Text)
	{
		m_Text->Release();
		delete m_Text;
		m_Text = 0;
	}
	// Release the frustum
	if(m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}
	// Release the enemy list
	if(m_EnemyList)
	{
		m_EnemyList->Release();
		delete m_EnemyList;
		m_EnemyList = 0;
	}
	// Release the terrain
	if(m_WorldTerrain)
	{
		m_WorldTerrain->Release();
		delete m_WorldTerrain;
		m_WorldTerrain = 0;
	}
	// Release the color shader
	if(m_colorShader)
	{
		m_colorShader->Release();
		delete m_colorShader;
		m_colorShader = 0;
	}
	// Release the terrain shader
	if(m_terrainShader)
	{
		m_terrainShader->Release();
		delete m_terrainShader;
		m_terrainShader = 0;
	}
	// Release the quad tree
	if(m_quadTree)
	{
		m_quadTree->Release();
		delete m_quadTree;
		m_quadTree = 0;
	}
	// Release the sky shader
	if(m_skyShader)
	{
		m_skyShader->Release();
		delete m_skyShader;
		m_skyShader = 0;
	}
	// Release the sky box
	if(m_skyBox)
	{
		m_skyBox->Release();
		delete m_skyBox;
		m_skyBox = 0;
	}

	return;
}


