
/*
******************************************************************
* ASFont.cpp
*******************************************************************
* Implements all methods from ASFont.h
*******************************************************************
*/
	
#include "ASFont.h"

/*
*******************************************************************
* Constructor
*******************************************************************
* Initialise pointers to null
*/

ASFont::ASFont()
{
	m_font = 0;
	m_texture = 0;
}

/*
*******************************************************************
* Empty Constructor
*******************************************************************
*/

ASFont::ASFont(const ASFont&)
{}

/*
*******************************************************************
* Destructor
*******************************************************************
*/

ASFont::~ASFont()
{}

/*
*******************************************************************
* METHOD: Init 
*******************************************************************
* Initialise the font object, this will load the font texture
* and populate the data structs to be sent to the vertex shader
* for rendering
*
* @param ID3D11Device* - pointer to our rendering device
* @param char*		   - name of the font file to load
* @param WCHAR*		   - name of the texture to be mapped to the font
*
* @return bool - True if successfully mapped, else false
*/

bool ASFont::Init(ID3D11Device* device, char* fontFile, WCHAR* texFile)
{
	// Attempt to load the font file
	bool success = LoadFont(fontFile);
	if(!success)
		return false;

	// Attempt to load the font texture
	success = LoadTexture(device, texFile);
	if(!success)
		return false;

	return true;
}

/*
*******************************************************************
* METHOD: Load Font 
*******************************************************************
* Loads the font index data (map of where each char is in the font img)
* to the ASFontType structure, this is then later sent to the buffer
* for rendering 
*
* @param char* - the filename we are loading
* @return bool - true if loaded, else false
*/

bool ASFont::LoadFont(char* fontFile)
{
	ifstream fileIn;;
	char readChar;

	// Create a new ASFontType array with characters equal to the number of elems in the file
	m_font = new ASFontType[95];
	if(!m_font)
		return false;

	// Open the file and read each line into the global pointer array. 
	// this will read the texture coords and size of the pixel, we only want the 
	// last 3 characters of the file so skip all the way to the end
	fileIn.open(fontFile);
	if(fileIn.fail())
		return false;

	// Get the last 3 digits from the file
	for(unsigned int i = 0; i < 95; i++)
	{
		fileIn.get(readChar);
		while(readChar != ' ')
			fileIn.get(readChar);
		
		fileIn.get(readChar);
		while(readChar != ' ')
			fileIn.get(readChar);

		// Get the last 3 characters (we know the structure of the layout hence why this algorithm works)
		fileIn >> m_font[i].left;
		fileIn >> m_font[i].right;
		fileIn >> m_font[i].size;
	}

	// Close file and return true
	fileIn.close();
	return true;
}

/*
*******************************************************************
* METHOD: Load Texture 
*******************************************************************
* Loads the texture to render to the font, populating VS buffers
*
* @param ID3D11Device* - Pointer to the rendering device
* @param WCHAR*        - Pointer to the filename of the texture
*
* @return bool - true if loaded, else false
*/

bool ASFont::LoadTexture(ID3D11Device* device, WCHAR* textureFile)
{
	m_texture = new ASTexture;
	if(!m_texture)
		return false;

	// Initialise the texture object
	bool success = m_texture->Init(device, textureFile);
	if(!success)
		return false;

	return true;
}

/*
*******************************************************************
* METHOD: Get Texture 
*******************************************************************
* Adapter to get texture back from the ASTexture class
*/

ID3D11ShaderResourceView* ASFont::GetTexture()
{
	return m_texture->GetTexture();
}

/*
*******************************************************************
* METHOD: Build Vertex Array 
*******************************************************************
* Builds the vertex array to be sent to the shader. This method will
* be called by ASText to construct sentences.  
*
* @param void* - total number of vertices 
* @param char* - the sentence to be constructed 
* @param float - where to draw at location X
* @param float - where to draw at location Y
*/

void ASFont::BuildVertexArray(void* vertices, char* sentence, float posX, float posY)
{
	ASVertex* vPtr;
	int numLetters;
	int letter;
	int index = 0;

	// Cast the input vertices to the vertex pointer
	vPtr = (ASVertex*)vertices;

	// Get the number of letts for the input setnence
	numLetters = (int)strlen(sentence);

	// Draw each letter onto the output panel
	for(unsigned int i = 0; i < numLetters; i++)
	{
		// Get the letter
		letter = ((int)sentence[i]) - 32;

		// If we encounter a space, add 3 pixels of space between chars
		if(letter == 0)
		{
			posX += 3.0f;
		} 
		else 
		{
			// First triangle in quad.
			vPtr[index].pos = D3DXVECTOR3(posX, posY, 0.0f);  // Top left.
			vPtr[index].tex = D3DXVECTOR2(m_font[letter].left, 0.0f);
			index++;

			vPtr[index].pos = D3DXVECTOR3((posX + m_font[letter].size), (posY - 16), 0.0f);  // Bottom right.
			vPtr[index].tex = D3DXVECTOR2(m_font[letter].right, 1.0f);
			index++;

			vPtr[index].pos = D3DXVECTOR3(posX, (posY - 16), 0.0f);  // Bottom left.
			vPtr[index].tex = D3DXVECTOR2(m_font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vPtr[index].pos = D3DXVECTOR3(posX, posY, 0.0f);  // Top left.
			vPtr[index].tex = D3DXVECTOR2(m_font[letter].left, 0.0f);
			index++;

			vPtr[index].pos = D3DXVECTOR3(posX + m_font[letter].size, posY, 0.0f);  // Top right.
			vPtr[index].tex = D3DXVECTOR2(m_font[letter].right, 0.0f);
			index++;

			vPtr[index].pos = D3DXVECTOR3((posX + m_font[letter].size), (posY - 16), 0.0f);  // Bottom right.
			vPtr[index].tex = D3DXVECTOR2(m_font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			posX = posX + m_font[letter].size + 1.0f;
		}
	}

	return;
}

/*
*******************************************************************
* METHOD: Release 
*******************************************************************
* Public interface to release all objects associated with ASFont
*/

void ASFont::Release()
{
	// Release the font object
	ReleaseFont();
	// Release texture object
	ReleaseTexture();
}

/*
*******************************************************************
* METHOD: Release Font
*******************************************************************
*/

void ASFont::ReleaseFont()
{
	if(m_font)
	{
		delete [] m_font;
		m_font = 0;
	}
}

/*
*******************************************************************
* METHOD: Release Texture
*******************************************************************
*/

void ASFont::ReleaseTexture()
{
	if(m_texture)
	{
		m_texture->Release();
		delete m_texture;
		m_texture = 0;
	}
}