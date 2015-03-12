
/*
******************************************************************
* ASEngine.h
*******************************************************************
* Encapsulates the engine logic, allowing us to separate it from
* the main.cpp file
*******************************************************************
*/

#ifndef _ASENGINE_H_
#define _ASENGINE_H_

/*
******************************************************************
* Pre-processing directives
******************************************************************
*/

#define WIN32_LEAN_AND_MEAN  // reduces compile time by excluding redundant APIs from build

/*
******************************************************************
* Includes
******************************************************************
* @Windows.h    - Used to spawn and destroy Windows using the Win API
* @ASInput.h    - Used to handle input events such as keyboard and mouse
* @ASGraphics.h - Used to 
******************************************************************
*/

#include <windows.h>
#include "ASInput.h"
#include "ASGraphics.h"

/*
******************************************************************
* Class declaration
******************************************************************
*/

class ASEngine 
{
public:
	// Constructors / Destructor prototype
	ASEngine();
	ASEngine(const ASEngine&);
	~ASEngine();

	// Public methods
	bool Init();
	void Release();
	void Run();

	// WINAPI Callback to handle inputs
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	// Private methoder
	bool DispatchASEvent();
	void InitASWindow(int&, int&);
	void CloseASWindow();

private:
	// Private member variables
	LPCWSTR     m_appName;
	HINSTANCE   m_hInstance;
	HWND        m_hwnd;

	ASInput*    m_input;
	ASGraphics* m_graphics;
};

/*
******************************************************************
* Global declarations
******************************************************************
* These methods are not directly used by the ASEngine instance,
* but it makes sense to put them in this class as they dispatch
* messages to the class through its handle
******************************************************************
*/

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ASEngine instance handle (can only ever be one instance in memory)
static ASEngine* EngineInstance = 0;

#endif