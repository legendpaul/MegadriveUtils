/**
 * @file transitions.h
 * @brief Header file for screen transition effects.
 *
 * This module provides functions for common screen transition effects,
 * such as fading the screen to black and fading in from black. These are
 * useful for changing scenes or states within a game.
 */
#ifndef TRANSITIONS_H
#define TRANSITIONS_H

#include <genesis.h> // SGDK general header

/**
 * @brief Stores the current VDP palettes (PAL0 and PAL1) for later restoration.
 *
 * This function reads the color values from hardware palettes PAL0 and PAL1
 * and stores them in static arrays within `transitions.c`. This allows
 * these palettes to be restored correctly after a fade-out or before a fade-in.
 * If PAL2 and PAL3 are also used by the application and need to be preserved
 * during transitions that affect all palettes (like `VDP_fadeOutAll`),
 * they should also be stored here.
 */
void store_current_palettes();

/**
 * @brief Fades the entire screen (all palettes) out to black.
 *
 * This function uses SGDK's `VDP_fadeOutAll()` to gradually change all
 * 64 VDP color entries (across PAL0, PAL1, PAL2, PAL3) to black (0x000).
 * It then waits for a VSync to ensure the fade effect is complete before returning.
 *
 * @param speed_frames The duration of the fade effect in frames.
 *                     A value of 1 is very fast, higher values are slower.
 *                     For example, 30 frames is 0.5 seconds on a 60Hz system.
 */
void transition_fade_out_to_black(u16 speed_frames);

/**
 * @brief Fades the screen in from black to the previously stored palettes.
 *
 * This function uses SGDK's `VDP_fadeInAll()`. It constructs a 64-color
 * palette array using the palettes stored by `store_current_palettes()`
 * (typically `stored_palette0` and `stored_palette1`). PAL2 and PAL3 are
 * currently assumed to fade in to black unless they were also stored and
 * are explicitly copied into the target palette array in `transitions.c`.
 * It then waits for a VSync to ensure the fade effect is complete.
 *
 * @param speed_frames The duration of the fade effect in frames.
 */
void transition_fade_in_from_black(u16 speed_frames);

#endif // TRANSITIONS_H
