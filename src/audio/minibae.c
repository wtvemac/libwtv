#include "audio.h"
#include "audio/minibae.h"
#include "libc.h"
#include "wtvsys.h"

BAEMixer bae_mixer                = NULL;
void*    audio_buffer1            = NULL;
void*    audio_buffer2            = NULL;
void*    playing_audio_buffer     = NULL;

audio_callback_data __sound_callback()
{
	audio_callback_data ret;

	if(playing_audio_buffer != NULL)
	{
		BAE_BuildMixerSlice(NULL, playing_audio_buffer, AUDIO_BUFFER_SIZE, SAMPLES_PER_BUFFER);

		ret = (audio_callback_data) {
			.buffer = playing_audio_buffer,
			.length = AUDIO_BUFFER_SIZE
		};

		if(playing_audio_buffer == audio_buffer1)
		{
			playing_audio_buffer = audio_buffer2;
		}
		else
		{
			playing_audio_buffer = audio_buffer1;
		}
	}
	else
	{
		ret = (audio_callback_data) {
			.buffer = NULL,
			.length = 0
		};
	}

	return ret;
}

BAEMixer minibae_init()
{
	bae_mixer = BAEMixer_New();

	BAEResult r1 = BAEMixer_Open(bae_mixer, BAE_AUDIO_SAMPLE_RATE, BAE_INTERPOLATION, BAE_AUDIO_FLAGS, BAE_MAX_SONG_VOICES, BAE_MAX_SOUND_VOICES, BAE_MIX_LEVEL, true);

	if(r1 != BAE_NO_ERROR)
	{
		minibae_close();

		return NULL;
	}
	else
	{
		return bae_mixer;
	}
}

void minibae_close()
{
	if(bae_mixer != NULL)
	{
		BAEMixer_Delete(bae_mixer);
	}
}

int BAE_Setup(void)
{
	return 0;
}

int BAE_Cleanup(void)
{
	return 0;
}

void *BAE_Allocate(unsigned long size)
{
	void* data = malloc(size);

	memset(data, 0, size);

	return data;
}

void BAE_Deallocate(void *memoryBlock)
{
	free(memoryBlock);
}

unsigned long BAE_GetSizeOfMemoryUsed(void)
{
	return 0;
}

unsigned long BAE_GetMaxSizeOfMemoryUsed(void)
{
	return 0;
}

int BAE_IsBadReadPointer(void *memoryBlock, unsigned long size)
{
	return 2;
}

unsigned long BAE_SizeOfPointer(void *memoryBlock)
{
	return 0;
}

void BAE_BlockMove(void *source, void *dest, unsigned long size)
{
	memmove(dest, source, size);
}

int BAE_IsStereoSupported(void)
{
	return 1;
}

int BAE_Is16BitSupported(void)
{
	return 1;
}

int BAE_Is8BitSupported(void)
{
	return 1;
}

short int BAE_GetHardwareBalance(void)
{
	return 0;
}

void BAE_SetHardwareBalance(short int balance)
{
}

short int BAE_GetHardwareVolume(void)
{
	return 0;
}

void BAE_SetHardwareVolume(short int newVolume)
{
}

unsigned long BAE_Microseconds(void)
{
	return get_ticks_us();
}

void BAE_WaitMicroseconds(unsigned long _wait_us)
{
	wait_us(_wait_us);
}

int BAE_NewMutex(BAE_Mutex* lock, char *name, char *file, int lineno)
{
	return 1;
}

void BAE_AcquireMutex(BAE_Mutex lock)
{
}

void BAE_ReleaseMutex(BAE_Mutex lock)
{
}

void BAE_DestroyMutex(BAE_Mutex lock)
{
}

void BAE_Idle(void *userContext)
{
}

void BAE_CopyFileNameNative(void *fileNameSource, void *fileNameDest)
{
}

long BAE_FileCreate(void *fileName)
{
	return -1;
}

long BAE_FileDelete(void *fileName)
{
	return -1;
}

long BAE_FileOpenForRead(void *fileName)
{
	return -1;
}

long BAE_FileOpenForWrite(void *fileName)
{
	return -1;
}

long BAE_FileOpenForReadWrite(void *fileName)
{
	return -1;
}

void BAE_FileClose(long fileReference)
{
}

long BAE_ReadFile(long fileReference, void *pBuffer, long bufferLength)
{
	return -1;
}

long BAE_WriteFile(long fileReference, void *pBuffer, long bufferLength)
{
	return -1;
}

long BAE_SetFilePosition(long fileReference, unsigned long filePosition)
{
	return -1;
}

unsigned long BAE_GetFilePosition(long fileReference)
{
	return 0;
}

unsigned long BAE_GetFileLength(long fileReference)
{
	return 0;
}

int BAE_SetFileLength(long fileReference, unsigned long newSize)
{
	return -1;
}

void BAE_ProcessRouteBus(int currentRoute, long *pChannels, int count)
{
}

int BAE_GetAudioBufferCount(void)
{
	return 2;
}

long BAE_GetAudioByteBufferSize(void)
{
	return AUDIO_BUFFER_SIZE;
}

int BAE_Mute(void)
{
	audio_pause(true);

	return 0;
}

int BAE_Unmute(void)
{
	audio_pause(false);

	return 0;
}

int BAE_AquireAudioCard(void *threadContext, unsigned long sampleRate, unsigned long channels, unsigned long bits)
{
	if(channels == 2)
	{
		if(bits == 16)
		{
			audio_buffer1 = malloc(AUDIO_BUFFER_SIZE);
			audio_buffer2 = malloc(AUDIO_BUFFER_SIZE);

			playing_audio_buffer = audio_buffer1;

			audio_init((int)sampleRate, -1);
			audio_set_outx_buffer_callback(__sound_callback);

			int resultFreq = audio_get_frequency();
			if(resultFreq != (int)sampleRate)
			{
				printf("Audio will start but have mismatched sample rate. Requested sampleRate=%d, result sampleRate=%d\n", (int)sampleRate, resultFreq);
			}
		}
		else
		{
			printf("Incorrect number of bits per sample requested. bits=%d need 16\n", (int)bits);
		}
	}
	else
	{
		printf("Incorrect number of audio channels requested. channels=%d need 2\n", (int)channels);
	}

	return 0;
}

int BAE_ReleaseAudioCard(void *threadContext)
{
	audio_close();
	
	playing_audio_buffer = NULL;

	free(audio_buffer1);
	free(audio_buffer2);

	return 0;
}

unsigned long BAE_GetDeviceSamplesPlayedPosition(void)
{
	return 0;
}

long BAE_MaxDevices(void)
{
	return 1;
}

void BAE_SetDeviceID(long deviceID, void *deviceParameter)
{
}

long BAE_GetDeviceID(void *deviceParameter)
{
	return 0;
}

void BAE_GetDeviceName(long deviceID, char *cName, unsigned long cNameLength)
{
}
