
#ifndef __LIBWTV_AUDIO_MINIBAE_H
#define __LIBWTV_AUDIO_MINIBAE_H

#include "X_API.h"
#include "MiniBAE.h"
#include "BAE_API.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BAE_AUDIO_SAMPLE_RATE BAE_RATE_44K
#define BAE_INTERPOLATION     BAE_LINEAR_INTERPOLATION
#define BAE_AUDIO_FLAGS       BAE_USE_16 | BAE_USE_STEREO
#define BAE_MAX_SOUND_VOICES  1
#define BAE_MAX_SONG_VOICES   (BAE_MAX_VOICES - BAE_MAX_SOUND_VOICES)
#define BAE_MIX_LEVEL         BAE_MAX_SONG_VOICES / 3

BAEMixer minibae_init();
void minibae_close();

#ifdef __cplusplus
}
#endif

#endif