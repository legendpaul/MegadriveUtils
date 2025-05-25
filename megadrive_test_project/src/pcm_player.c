#include "pcm_player.h"
#include "sound_manager.h" // To check if the sound system is initialized
#include "error_handler.h" // For reporting errors

// Define a module name for error messages
#define MODULE_NAME_PCM_PLAYER "pcm_player"

/**
 * @brief Plays a PCM sound effect using SGDK's sound functions.
 * 
 * Before attempting to play the sound, this function performs several checks:
 * 1. Verifies that the sound manager (sound_manager.c) has been initialized.
 * 2. Checks if the provided `sound_data` pointer is NULL.
 * 3. Checks if the `sound_data->len` (length of the sound) is zero. If so,
 *    it silently returns as playing a zero-length sound is not an error but
 *    would have no effect.
 * 
 * If any critical checks fail (sound manager not ready, NULL data pointer),
 * it calls `error_handler_display_error()` to halt the program and display
 * an error message.
 * 
 * @param sound_data A pointer to a `PCM` structure containing the sound data
 *                   (samples, length, rate). This structure is typically generated
 *                   by SGDK's `rescomp` tool from .wav or .raw sound files defined
 *                   in the `resources.res` file.
 */
void pcm_player_play(const PCM* sound_data) {
    // Check 1: Is the sound manager initialized?
    if (!sound_manager_is_initialized()) {
        error_handler_display_error(MODULE_NAME_PCM_PLAYER, __func__, __LINE__, 
                                    "Sound manager not initialized!");
        return; // error_handler_display_error should halt, but return for safety
    }

    // Check 2: Is the sound_data pointer NULL?
    if (sound_data == NULL) {
        error_handler_display_error(MODULE_NAME_PCM_PLAYER, __func__, __LINE__, 
                                    "PCM sound_data pointer is NULL!");
        return;
    }

    // Check 3: Is the sound length zero?
    // A zero-length sound isn't a fatal error, just means nothing to play.
    // No error message, just return silently.
    if (sound_data->len == 0) {
        // KLog("PCM data length is 0, not playing."); // Optional: KLog for debugging
        return; 
    }

    // All checks passed, attempt to play the sound
    SND_startPlay_PCM(sound_data->data, sound_data->len, sound_data->rate, 
                      SND_PAN_CENTER, FALSE /* no loop */);
}
