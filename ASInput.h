
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

class ASInput 
{
public:
	// Constructors and Destructor prototypes
	ASInput();
	ASInput(const ASInput&);
	~ASInput();

	// Initialise method
	void Init();

	// Handle key presses
	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	void LeftMBDown();
	void LeftMBUp();

	// Flag to check if mouse or key is down
	bool IsKeyDown(unsigned int);
	bool IsMBDown();

private:
	bool m_keys[256];
	bool m_lMbDown;
};

#endif