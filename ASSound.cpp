
/*
******************************************************************
* ASSound.cpp
******************************************************************
* Implements all methods prototyped in ASSound.h
******************************************************************
*/

#include "ASSound.h"

/*
******************************************************************
* Constructor
******************************************************************
* Set all member values to 0.0f so we know our matrix has some
* default values
******************************************************************
*/

ASSound::ASSound()
{
	m_DirectSound	   = 0;
	m_primaryBuffer    = 0;
	m_secondaryBuffer  = 0;
}

/*
******************************************************************
* Constructor
******************************************************************
*/

ASSound::ASSound(const ASSound& sound)
{}

/*
******************************************************************
*  Destructor
******************************************************************
*/

ASSound::~ASSound()
{}

/*
******************************************************************
* METHOD: Init
******************************************************************
* Initialise the sound buffer and DirectSound API, once these
* have been initialised the the sound file can be loaded into the
* struct, the secondary buffer can then be populated with the
* data from the sound file, which can then be played through PlayFile()
*
* @return bool - true if sound was played, else false
******************************************************************
*/

bool ASSound::Init(HWND handle)
{
	// Initialise Direct Sound API
	bool success = InitDirectSound(handle);
	if(!success)
		return false;

	// Load the sound file
	success = LoadWavFile("./sounds/sound01.wav", &m_secondaryBuffer);
	if(!success)
		return false;

	// Play the loaded sound file (circular buffer so will loop)
	success = PlayFile();
	if(!success)
		return false;

	return true;
}

/*
******************************************************************
* METHOD: Init Direct Sound
******************************************************************
* Initialise the DirectSound API to stream the loaded file in a
* circular buffer - this method assigns a pointer to the primary
* sound buffer that has data streamed into it, the secondary
* buffer is then loaded via this.
******************************************************************
*/

bool ASSound::InitDirectSound(HWND handle)
{
	HRESULT hr;
	DSBUFFERDESC bDesc;
	WAVEFORMATEX wavFormat;

	// Initialise Direct sound interface 
	hr = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if(FAILED(hr))
		return false;

	// Allow the primary sound buffer to be modified
	hr = m_DirectSound->SetCooperativeLevel(handle, DSSCL_PRIORITY);
	if(FAILED(hr))
		return false;

	// Describe the sound buffer - this tells DX how we want to access the
	// primary buffer. 
	bDesc.dwSize = sizeof(DSBUFFERDESC);
	bDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bDesc.dwBufferBytes = 0;
	bDesc.dwReserved = 0;
	bDesc.lpwfxFormat = NULL;
	bDesc.guid3DAlgorithm = GUID_NULL;

	// Populate the primary buffer with the buffer descriptor
	hr = m_DirectSound->CreateSoundBuffer(&bDesc, &m_primaryBuffer, NULL);
	if(FAILED(hr))
		return false;

	// Configure the primary buffer now that it has been set.  Set the format and quality of the output sound here
	wavFormat.wFormatTag = WAVE_FORMAT_PCM;
	wavFormat.nSamplesPerSec = 44100;
	wavFormat.wBitsPerSample = 16;
	wavFormat.nChannels = 2;
	wavFormat.nBlockAlign = (wavFormat.wBitsPerSample / 8) * wavFormat.nChannels;
	wavFormat.nAvgBytesPerSec = wavFormat.nSamplesPerSec * wavFormat.nBlockAlign;
	wavFormat.cbSize = 0;

	// Set the primary buffer format with the modified desscriptor
	hr = m_primaryBuffer->SetFormat(&wavFormat);
	if(FAILED(hr))
		return false;

	return true;
}

/*
******************************************************************
* METHOD: Load Wav File
******************************************************************
* Loads the given wav file into the secondary buffer.  
*
* @return bool - true if loaded, else false
******************************************************************
*/

bool ASSound::LoadWavFile(char* filename, IDirectSoundBuffer8** buffer)
{
	int err;
	FILE* filePtr;
	unsigned int count;
	ASWavHeader wavFile;
	WAVEFORMATEX wavFormat;
	DSBUFFERDESC bDesc;
	HRESULT hr;
	IDirectSoundBuffer* tmpBuffer;
	unsigned char* wavData;
	unsigned char* buffPtr;
	unsigned long  buffSize;

	// Open the wav file and start to read in the header info to populate the ASWavHeader struct
	err = fopen_s(&filePtr, filename, "rb");
	if(err != 0)
		return false;

	// Read the header
	count = fread(&wavFile, sizeof(wavFile), 1, filePtr);
	if(count != 1)
		return false;

	// Check that the chunk ID is the RIFF format.
	if((wavFile.chunkId[0] != 'R') || (wavFile.chunkId[1] != 'I') || 
	   (wavFile.chunkId[2] != 'F') || (wavFile.chunkId[3] != 'F'))
		return false;
 
	// Check that the file format is the WAVE format.
	if((wavFile.format[0] != 'W') || (wavFile.format[1] != 'A') ||
	   (wavFile.format[2] != 'V') || (wavFile.format[3] != 'E'))
		return false;
 
	// Check that the sub chunk ID is the fmt format.
	if((wavFile.subChunkId[0] != 'f') || (wavFile.subChunkId[1] != 'm') ||
	   (wavFile.subChunkId[2] != 't') || (wavFile.subChunkId[3] != ' '))
		return false;
 
	// Check that the audio format is WAVE_FORMAT_PCM.
	if(wavFile.audioFormat != WAVE_FORMAT_PCM)
		return false;
 
	// Check that the wave file was recorded in stereo format.
	if(wavFile.numChannels != 2)
		return false;

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if(wavFile.sampleRate != 44100)
		return false;
 
	// Ensure that the wave file was recorded in 16 bit format.
	if(wavFile.bitsPerSample != 16)
		return false;
 
	// Check for the data chunk header.
	if((wavFile.dataChunkId[0] != 'd') || (wavFile.dataChunkId[1] != 'a') ||
	   (wavFile.dataChunkId[2] != 't') || (wavFile.dataChunkId[3] != 'a'))
		return false;

	// Above WAV file is valid, populat ethe secondaryBuffer with the audio information
	wavFormat.wFormatTag = WAVE_FORMAT_PCM;
	wavFormat.nSamplesPerSec = 44100;
	wavFormat.wBitsPerSample = 16;
	wavFormat.nChannels = 2;
	wavFormat.nBlockAlign = (wavFormat.wBitsPerSample / 8) * wavFormat.nChannels;
	wavFormat.nAvgBytesPerSec = wavFormat.nSamplesPerSec * wavFormat.nBlockAlign;
	wavFormat.cbSize = 0;

	bDesc.dwSize = sizeof(DSBUFFERDESC);
	bDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bDesc.dwBufferBytes = wavFile.dataSize;
	bDesc.dwReserved = 0;
	bDesc.lpwfxFormat = &wavFormat;
	bDesc.guid3DAlgorithm = GUID_NULL;

	// Create the secondary buffer by creating a temp IDirectSoundBuffer and then create the second buffer by
	// querying thats buffer inerface - this will populate the secondary buffer, then the temp can be releaed
	hr = m_DirectSound->CreateSoundBuffer(&bDesc, &tmpBuffer, NULL);
	if(FAILED(hr))
		return false;

	// Query the buffer format against the DS8 interface, then creat the secondary buffer
	hr = tmpBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*buffer);
	if(FAILED(hr))
		return false;

	// Release the temp buffer
	tmpBuffer->Release();
	tmpBuffer = 0;

	// Load the audio data from the wav file into the newly created buffer.  
	fseek(filePtr, sizeof(ASWavHeader), SEEK_SET);
	wavData = new unsigned char[wavFile.dataSize];
	if(!wavData)
		return false;

	// Read the wav file into the buffer
	count = fread(wavData, 1, wavFile.dataSize, filePtr);
	if(count != wavFile.dataSize)
		return false;

	// Finished reading - close the file.
	err = fclose(filePtr);
	if(err != 0)
		return false;

	// Lock the secondary buffr and write the data just read into it
	hr = (*buffer)->Lock(0, wavFile.dataSize, (void**)&buffPtr, (DWORD*)&buffSize, NULL, 0, 0);
	if(FAILED(hr))
		return false;

	memcpy(buffPtr, wavData, wavFile.dataSize);

	// Unlock the buffer once it has been written to
	hr = (*buffer)->Unlock((void*)buffPtr, buffSize, NULL, 0);
	if(FAILED(hr))
		return false;

	// Release the wav data struct as it now exists in the secondary buffer
	delete [] wavData;
	wavData = 0;

	return true;
}

/*
******************************************************************
* METHOD: Play File
******************************************************************
* Play the sound file loaded into the secondary buffer, once
* this method is called the secondary buffers data is loaded into 
* the primary buffer from where it is streamed
******************************************************************
*/

bool ASSound::PlayFile()
{
	HRESULT hr;

	// Start from beginning
	hr = m_secondaryBuffer->SetCurrentPosition(0);
	if(FAILED(hr))
		return false;

	// Set volume of file
	hr = m_secondaryBuffer->SetVolume(DSBVOLUME_MAX);
	if(FAILED(hr))
		return false;

	// Play the sound contents
	hr = m_secondaryBuffer->Play(0,0,0);
	if(FAILED(hr))
		return false;

	return true;
}

/*
******************************************************************
* METHOD: Release
******************************************************************
* Call private release methods to dispose of any resources held by
* the ASSound class
******************************************************************
*/

void ASSound::Release()
{
	// Release the file being played
	ReleaseFile(&m_secondaryBuffer);
	// Release the DirectSound API
	ReleaseDirectSound();
}

/*
******************************************************************
* METHOD: Release File
******************************************************************
* Dispose of any data held on the file, including its circular
* buffer
******************************************************************
*/

void ASSound::ReleaseFile(IDirectSoundBuffer8** buffer)
{
	// Release the primary sound buffer ptr
	if(*buffer)
	{
		(*buffer)->Release();
		(*buffer) = 0;
	}
}

/*
******************************************************************
* METHOD: Release Direct Sound
******************************************************************
* Dispose of any methods held by the DS object
******************************************************************
*/

void ASSound::ReleaseDirectSound()
{
	// Release the primary sound buffer ptr
	if(m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}
	// Release the DS interface pointer
	if(m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}
}