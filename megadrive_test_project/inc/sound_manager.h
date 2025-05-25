#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <genesis.h> // For u8

// Initializes the sound manager.
// For current PCM usage, this primarily sets an internal flag,
// as SGDK_init() handles basic Z80 setup.
void sound_manager_init(void);

// Checks if the sound manager has been initialized.
// Returns TRUE if initialized, FALSE otherwise.
u8 sound_manager_is_initialized(void);

#endif // SOUND_MANAGER_H
