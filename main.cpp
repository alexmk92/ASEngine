
/*
******************************************************************
* Main.cpp
*******************************************************************
* Main entry point for the program, this is intentionally light-weight
* to ensure that all heavy lifting is carried out by the Engine
* this class will safely create an ASEngine instance, if anything
* goes wrong it will clean up gracefully by releasing objects
*******************************************************************
*/

#include "ASEngine.h";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdline, int iCmdShow)
{
	ASEngine *Engine;
	bool success;

	// Create a new instance of ASEngine, then check it has been initialised, if a
	// a null pointer is returned then quit out of the program (this shouldn't happen)
	// but it's better to be safe than sorry.
	Engine = new ASEngine;
	if(!Engine) 
		return 0;

	// Initialize the system, if successful enter the main game loop, otherwise despose
	// of the Engine and exit the program gracefully - disposing of any resources
	success = Engine->Init();
	if(success)
		Engine->Run();

	// Clean up - dispose of any resources held by Engine and then delete it
	// Ensure we can't access it again by assigning 0 to the Engine variable, this
	// will set a null pointer to it in memory - we can then return 0 to exit out 
	// of the program
	Engine->Release();
	delete Engine;
	Engine = 0;

	return 0;
}