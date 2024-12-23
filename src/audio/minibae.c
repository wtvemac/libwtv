#include "audio.h"
#include "audio/minibae.h"

void minibae_init()
{
	audio_init(44100, 2);

	BAEMixer test = BAEMixer_New();
}

void minibae_close()
{
	audio_close();
}