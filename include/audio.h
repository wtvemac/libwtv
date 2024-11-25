#ifndef __LIBWTV_AUDIO_H
#define __LIBWTV_AUDIO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t WriteOutputsEMAC(uint32_t a, bool b);
uint32_t SetAudioMuteEMAC(bool a);
uint32_t SetAudioCodecCS_NEMAC(bool a);
uint32_t SetAudioCodecCCLKEMAC(bool a);
uint32_t SetAudioCodecCDATAEMAC(bool a);
uint32_t WriteCodecControlEMAC(uint32_t a, uint32_t b);
uint32_t InitAudioCodecEMAC();

void clear_audio();
void audio_init();
void play_poopie();
void progress_audio();

#ifdef __cplusplus
}
#endif

#endif