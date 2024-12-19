#ifndef __LIBWTV_AUDIO_H
#define __LIBWTV_AUDIO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef short asamp;

#define DEFAULT_AUDIO_CLOCK 44100

void audio_init(int frequency, int numbuffers);
void audio_write_temp(void* buffer, uint32_t length); // This is temporary for testing.
void audio_close();

#ifdef __cplusplus
}
#endif

#endif