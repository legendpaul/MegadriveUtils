/**
 * @file sound.h
 * @brief Header file for sound effect playback functions.
 *
 * This module handles the initialization of the sound system and playback
 * of PCM (Pulse Code Modulation) sound effects. Sound effects are defined
 * in `resources.res` and compiled by SGDK's `rescomp` tool, making them
 * available via `resources.h`.
 */
#ifndef SOUND_H
#define SOUND_H

#include <genesis.h> // SGDK general header

// --- PCM Sound Effect Data (defined in sound.c) ---
// These extern declarations make the hardcoded sound data (defined in sound.c)
// accessible to other parts of the application if needed, though typically
// only sound.c itself and the sound playback functions use them directly.
//
// The hardcoded data has been removed and replaced by a resource compiled
// via rescomp. The 'PCM sfx_ping_data' struct will be available from 'resources.h'.
// extern const u8 pcm_beep_sound_data[]; // REMOVED
// extern const u32 pcm_beep_sound_len;    // REMOVED
// extern const SoundRate pcm_beep_sound_rate; // REMOVED


// --- Sound Functions ---

/**
 * @brief Initializes the sound system.
 *
 * This function should be called once at application startup to prepare
 * the sound hardware or drivers. For basic PCM playback via `SND_startPlay_PCM`,
 * SGDK's main `SGDK_init()` (called in `main.c`) typically handles necessary
 * Z80 setup. This function can be expanded if more advanced sound drivers
 * (like XGM for music) are used.
 */
void init_sound_system();

/**
 * @brief Plays the "ping" (beep) sound effect.
 *
 * This function triggers the playback of the `sfx_ping_data` PCM resource,
 * which is defined in `resources.res` and compiled into `resources.h`.
 * The sound is played on the center channel without looping using SGDK's
 * `SND_startPlay_PCM` function.
 */
void play_sfx_ping();

// Placeholder for music functions (not currently implemented)
// void start_music_example();
// void stop_music_example();

#endif // SOUND_H
