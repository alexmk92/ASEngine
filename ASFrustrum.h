
/*
******************************************************************
* ASFrustrum.h
*******************************************************************
* Creates a frustrum class to determine what items are in the
* view to be rendered at any one time.  Without a frustrum, the 
* performance of the application could significantly dimish
*******************************************************************
*/

#ifndef _ASFRUSTRUM_H_
#define _ASFRUSTRUM_H_

/*
*******************************************************************
* Includes:
*******************************************************************
*/

#include <d3dx10math.h>

/*
*******************************************************************
* Class declaration
*******************************************************************
*/

class ASFrustrum
{
public:
	// Constructors and Destructors
	ASFrustrum();
	ASFrustrum(const ASFrustrum&);
	~ASFrustrum();

	// Public methods
	void ConstructFrustrum(float, D3DXMATRIX, D3DXMATRIX);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);

private:
	// Private member variables
	D3DXPLANE m_planes[6];
};

#endif