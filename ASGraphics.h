
/*
******************************************************************
* ASGraphics.h
*******************************************************************
* Handles all drawing events to the window, this class is 
* a delegate of ASEngine.h
*******************************************************************
*/

#ifndef _ASGRAPHICS_H_
#define _ASGRAPHICS_H_

/*
*******************************************************************
* Includes:
*******************************************************************
* + ASDirect3D.h has been included to handle D3D system functions 
* + ASCamera.h has been included so we can view the world around a camera
* + ASModel.h has been included to load meshes to be rendered to scene
* + ASColorShader.h has been included to apply color to loaded objects through VS and PS
*******************************************************************
*/

#include "ASDirect3D.h"
#include "ASCamera.h"
#include "ASModel.h"
#include "ASLightShader.h"
#include "ASLight.h"
#include "ASText.h"
#include "ASEnemies.h"
#include "ASPlayer.h"
#include "ASColorShader.h"
#include "ASQuadTree.h"
#include "ASSkyShader.h"
#include "ASSkyBox.h"
#include <vector>

/*
*******************************************************************
* Global configuration constants
*******************************************************************
*/

const bool  FULL_SCREEN   = false;
const bool  VSYNC_ENABLED = true;
const float SCREEN_DEPTH  = 1000.0f;
const float SCREEN_NEAR   = 1.25f;

// Spawn coordinates for the player in the world
const float SPAWN_X = 20.0f;
const float SPAWN_Y = 2.0f;
const float SPAWN_Z = 0.0f;

/*
*******************************************************************
* Class declaration
*******************************************************************
*/

class ASGraphics
{
public:
	// Struct to hold camera info
	struct ASCameraInfo
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot;
	};
	// Constructors and Destructors
	ASGraphics();
	ASGraphics(const ASGraphics&);
	~ASGraphics();

	// Public methods
	bool Init(int, int, HWND);
	bool RenderScene(ASCameraInfo);
	void Release();

private:
	// Private member variables
	ASColorShader*   m_colorShader;
	ASLightShader*   m_lightShader;
	ASTerrainShader* m_terrainShader;
	ASLight*         m_light;
	ASDirect3D*      m_D3D;
	ASCamera*        m_Camera;
	ASModel*         m_Model;
	ASText*          m_Text;
	ASFrustrum*      m_Frustum;
	ASEnemies*       m_EnemyList;
	ASTerrain*       m_WorldTerrain;
	ASPlayer*        m_player;
	ASQuadTree*      m_quadTree;
	ASSkyBox*        m_skyBox;
	ASSkyShader*     m_skyShader;
};

#endif