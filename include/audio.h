#ifndef __LIBWTV_AUDIO_H
#define __LIBWTV_AUDIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef short asamp;
#define audio_set_buffer_callback audio_set_out_buffer_callback

typedef struct {
    asamp* buffer;
    uint32_t length;
} audio_callback_data;

/** @brief Number of buffers the audio subsytem allocates and manages */
#define DEFAULT_AUDIO_BUFCNT 2
/** @brief How many bytes an audio sample holds in one channel. */
#define BYTES_PER_SAMPLE     (sizeof(asamp))
/** @brief The amount of audio channels to keep track of. */
#define AUDIO_CHANNEL_COUNT  2
/** @brief How many samples an audio buffer holds. 512 is about 11.6ms @ 44100Hz and 10.6ms @ 48000Hz  */
#define SAMPLES_PER_BUFFER   512
/** @brief The size in bytes to allocate for a single audio buffer. */
#define AUDIO_BUFFER_SIZE    ((BYTES_PER_SAMPLE * AUDIO_CHANNEL_COUNT) * SAMPLES_PER_BUFFER)

#define DEFAULT_AUDIO_CLOCK 44100

/**
 * @brief Will be called periodically when more sample data is needed.
 *
 * @param[in] buffer        The address to write the sample data to
 * @param[in] numsamples    The number of samples to write to the buffer
 * 
 * NOTE: This is the number of samples per channel, so clients should write
 * twice this number of samples (interleaved).
 */
typedef void (*audio_fill_out_buffer_callback)(asamp *buffer, size_t numsamples);
typedef audio_callback_data (*audio_fill_outx_buffer_callback)();

/**
 * @brief Initialize the audio subsystem
 *
 * This function will set up the AI to play at a given frequency and
 * allocate a number of back buffers to write data to.
 *
 * @note Before re-initializing the audio subsystem to a new playback
 *       frequency, remember to call #audio_close.
 *
 * @param[in] frequency
 *            The frequency in Hz to play back samples at
 * @param[in] numbuffers
 *            The number of buffers to allocate internally
 */
void audio_init(int frequency, int numbuffers);

/**
 * @brief Close the audio subsystem
 *
 * This function closes the audio system and cleans up any internal
 * memory allocated by #audio_init.
 */
void audio_close();

/**
 * @brief Install a audio callback to fill the audio buffer when required.
 * 
 * This function allows to implement a pull-based audio system. It registers
 * a callback which will be invoked under interrupt whenever the AI is ready
 * to have more samples enqueued. The callback can fill the provided audio
 * data with samples that will be enqueued for DMA to AI.
 * 
 * @param[in] fill_out_buffer_callback   Callback to fill an empty audio buffer
 */
void audio_set_out_buffer_callback(audio_fill_out_buffer_callback fill_out_buffer_callback);
void audio_set_outx_buffer_callback(audio_fill_outx_buffer_callback fill_outx_buffer_callback);

/**
 * @brief Pause or resume audio playback
 *
 * Should only be used when a fill_buffer_callback has been set
 * in #audio_init.
 * Silence will be generated while playback is paused.
 */
void audio_pause(bool pause);

/**
 * @brief Return whether there is an empty buffer to write to
 *
 * This function will check to see if there are any buffers that are not full to
 * write data to.  If all buffers are full, wait until the AI has played back
 * the next buffer in its queue and try writing again.
 */
volatile int audio_can_write();

/**
 * @brief Write a chunk of silence
 *
 * This function will write silence to be played back by the audio system.
 * It writes exactly #audio_get_buffer_length stereo samples.
 *
 * @note This function will block until there is room to write an audio sample.
 *       If you do not want to block, check to see if there is room by calling
 *       #audio_can_write.
 */
void audio_write_silence();

/**
 * @brief Return actual frequency of audio playback
 *
 * @return Frequency in Hz of the audio playback
 */
int audio_get_frequency();

/**
 * @brief Get the number of stereo samples that fit into an allocated buffer
 *
 * @note To get the number of bytes to allocate, multiply the return by
 *       2 * sizeof( asamp )
 *
 * @return The number of stereo samples in an allocated buffer
 */
int audio_get_buffer_length();

/**
 * @brief Get the number of bytes allocated in an audio buffer
 *
 * @return The number of bytes in an allocated buffer
 */
int audio_get_buffer_size();

/**
 * @brief Get the number of audio buffers alocated
 *
 * @return The number of buffers allocated for audio playback
 */
int audio_get_buffer_count();

/**
 * @brief Start writing to the first free internal buffer.
 * 
 * This function is similar to #audio_write but instead of taking samples
 * and copying them to an internal buffer, it returns the pointer to the
 * internal buffer. This allows generating the samples directly in the buffer
 * that will be sent via DMA to AI, without any subsequent memory copy.
 * 
 * The buffer should be filled with stereo interleaved samples, and
 * exactly #audio_get_buffer_length samples should be written.
 * 
 * After you have written the samples, call audio_write_end() to notify
 * the library that the buffer is ready to be sent to AI.
 * 
 * @note This function will block until there is room to write an audio sample.
 *       If you do not want to block, check to see if there is room by calling
 *       #audio_can_write.
 * 
 * @return  Pointer to the internal memory buffer where to write samples.
 */
asamp* audio_write_begin(void);

/**
 * @brief Complete writing to an internal buffer.
 * 
 * This function is meant to be used in pair with audio_write_begin().
 * Call this once you have generated the samples, so that the audio
 * system knows the buffer has been filled and can be played back.
 * 
 */
void audio_write_end(void);

/**
 * @brief Push a chunk of audio data (high-level function)
 * 
 * This function is an easy-to-use, higher level alternative to all
 * the audio_write* functions. It pushes audio samples into output
 * hiding the complexity required to match the fixed-size audio buffers.
 * 
 * The function accepts a @p buffer of stereo interleaved audio samples;
 * @p nsamples is the number of samples in the buffer. The function will
 * push the samples into output as much as possible.
 * 
 * If @p blocking is true, it will stop and wait until all samples have
 * been pushed into output. If @p blocking is false, it will stop as soon
 * as there are no more free buffers to push samples into, and will return
 * the number of pushed samples. It is up to the caller to then take care
 * of this and later try to call audio_push again with the remaining samples.
 * 
 * @note You CANNOT mixmatch this function with the other audio_write* functions,
 *       and viceversa. If you decide to use audio_push, use it exclusively to
 *       push the audio.
 * 
 * @param buffer        Buffer containing stereo samples to be played
 * @param nsamples      Number of stereo samples in the buffer
 * @param blocking      If true, wait until all samples have been pushed
 * @return int          Number of samples pushed into output
 */
int audio_push(const asamp *buffer, int nsamples, bool blocking);

__attribute__((deprecated("use audio_write_begin or audio_push instead")))
void audio_write(const asamp * const buffer);

#ifdef __cplusplus
}
#endif

#endif