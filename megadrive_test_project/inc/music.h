#ifndef MUSIC_H
#define MUSIC_H

#include <genesis.h> // For u8

// Initializes the XGM music driver
void music_init_driver(); // Renamed to avoid conflict if sound.c has init_sound_system

// Starts playing an XGM track by its resource ID
void music_start(u8 xgm_track_res_id);

// Stops XGM music playback
void music_stop();

// Updates the XGM driver (call this periodically, e.g., in VBlank or main loop)
void music_update_driver();

#endif // MUSIC_H
