
/*
******************************************************************
* ASSound.h
******************************************************************
* Encapsulates DirectSound 
******************************************************************
*/

#ifndef _ASSOUND_H_
#define _ASSOUND_H_

/*
******************************************************************
* Linker Vars
******************************************************************
*/

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

/*
******************************************************************
* Includes
******************************************************************
*/

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

/*
******************************************************************
* Class Declaration
******************************************************************
*/

class ASSound
{
private:
	// struct to encapsulate a .wav file, the header struct here is to
	// ensure that the file being imported to play through DX sound is
	// in the correct format, validation failures can cause app crashes.
	struct ASWavHeader
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};
	// struct to encapsulate a .mp3 file
	struct ASMP3Header 
	{
		// To be defined
	};

public:
	// Constructors and Destructors
	ASSound();
	ASSound(const ASSound&);
	~ASSound();

	// Public methods
	bool Init(HWND);
	void Release();

private:
	// Private methods
	bool InitDirectSound(HWND);
	void ReleaseDirectSound();

	bool LoadWavFile(char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
	void ReleaseFile(IDirectSoundBuffer8**, IDirectSound3DBuffer8**);

	bool PlayFile();

	// Private member variables
	IDirectSound8*		 m_DirectSound;
	IDirectSoundBuffer*  m_primaryBuffer;
	IDirectSoundBuffer8* m_secondaryBuffer;

	// Listener has been added, the listener determines where in the world
	// the 3D sound should be played
	IDirectSound3DListener8* m_listener;
	IDirectSound3DBuffer8*   m_secondary3DBuffer;
};

#endif