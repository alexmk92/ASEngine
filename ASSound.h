
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
	// struct to encapsulate a .wav file
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

	bool LoadWavFile(char*, IDirectSoundBuffer8**);
	void ReleaseFile(IDirectSoundBuffer8**);

	bool PlayFile();

	// Private member variables
	IDirectSound8*		 m_DirectSound;
	IDirectSoundBuffer*  m_primaryBuffer;
	IDirectSoundBuffer8* m_secondaryBuffer;
};

#endif