#ifndef PCM_PLAYER_H
#define PCM_PLAYER_H

#include <genesis.h> // For PCM struct definition (from sgdk/sound.h or types.h)

/**
 * @brief Plays a PCM sound effect.
 * 
 * This function attempts to play the provided PCM sound data.
 * It will perform checks to ensure the sound system is initialized
 * and the provided sound data is valid before attempting playback.
 * 
 * @param sound_data Pointer to the PCM struct (usually obtained from resources.h
 *                   after being processed by rescomp).
 */
void pcm_player_play(const PCM* sound_data);

#endif // PCM_PLAYER_H
