#include "sound_manager.h"
// No error_handler.h needed here if init itself is simple and can't fail critically.
// KLog could be used for debugging if available in the worker's environment.

static u8 is_initialized_flag = FALSE;

/**
 * @brief Initializes the sound manager.
 * 
 * For the current setup (basic PCM playback), SGDK_init() in main.c already handles
 * the necessary low-level initialization of the Z80 and sound hardware.
 * This function therefore primarily serves to set an internal flag indicating that
 * the sound manager considers itself ready. Future, more complex sound setups
 * (like initializing the XGM driver if it weren't already in music.c) could go here.
 */
void sound_manager_init(void) {
    is_initialized_flag = TRUE;
    // KLog("Sound manager initialized."); // Optional: For debugging on emulators
}

/**
 * @brief Checks if the sound manager has been initialized.
 * 
 * @return u8 TRUE if sound_manager_init() has been called, FALSE otherwise.
 *         This helps other modules ensure the sound system is ready before
 *         attempting to play sounds.
 */
u8 sound_manager_is_initialized(void) {
    return is_initialized_flag;
}
