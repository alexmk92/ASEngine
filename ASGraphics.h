
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
#include "ASColorShader.h"

/*
*******************************************************************
* Global configuration constants
*******************************************************************
*/

const bool  FULL_SCREEN   = false;
const bool  VSYNC_ENABLED = true;
const float SCREEN_DEPTH  = 1000.0f;
const float SCREEN_NEAR   = 0.1f;

/*
*******************************************************************
* Class declaration
*******************************************************************
*/

class ASGraphics
{
public:
	// Constructors and Destructors
	ASGraphics();
	ASGraphics(const ASGraphics&);
	~ASGraphics();

	// Public methods
	bool Init(int, int, HWND);
	bool UpdateFrame();
	void Release();

private:
	// Private methods
	bool RenderScene();

	// Private member variables
	ASColorShader* m_ColorShader;
	ASDirect3D*    m_D3D;
	ASCamera*      m_Camera;
	ASModel*       m_Model;

};

#endif