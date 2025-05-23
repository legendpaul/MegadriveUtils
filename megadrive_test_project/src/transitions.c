/**
 * @file transitions.c
 * @brief Implements screen transition effects.
 *
 * This module provides functions for fading the screen out to black and
 * fading back in from black. It uses SGDK's built-in VDP (Video Display Processor)
 * functions for palette manipulation to achieve these effects.
 * Palettes PAL0 and PAL1 are currently stored and restored.
 */
#include "transitions.h"
#include <string.h> // For memcpy, used in constructing the full palette for VDP_fadeInAll

/**
 * @brief Stored VDP hardware palette 0 (16 colors).
 * Used by `store_current_palettes()` and `transition_fade_in_from_black()`.
 */
static u16 stored_palette0[16];
/**
 * @brief Stored VDP hardware palette 1 (16 colors).
 * Used by `store_current_palettes()` and `transition_fade_in_from_black()`.
 */
static u16 stored_palette1[16];
// static const u16 palette_black_single[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// The above is not strictly needed because VDP_fadeOutAll() handles fading to black,
// and VDP_fadeInAll() can construct the target palette dynamically.

/**
 * @brief Stores the current VDP palettes (PAL0 and PAL1) into static arrays.
 *
 * This function reads the color values from hardware palettes PAL0 and PAL1
 * using `VDP_getPalette()` and saves them into `stored_palette0` and
 * `stored_palette1` respectively. This is essential for correctly restoring
 * these palettes during a fade-in transition.
 *
 * If the application also uses PAL2 and PAL3 and these need to be preserved
 * across fade transitions that affect all palettes (like `VDP_fadeOutAll`),
 * then `stored_palette2` and `stored_palette3` arrays should be added and
 * populated here as well.
 */
void store_current_palettes() {
    VDP_getPalette(PAL0, stored_palette0); // Get PAL0 colors
    VDP_getPalette(PAL1, stored_palette1); // Get PAL1 colors
    // If PAL2 and PAL3 were also in use and needed preservation:
    // VDP_getPalette(PAL2, stored_palette2);
    // VDP_getPalette(PAL3, stored_palette3);
}

/**
 * @brief Fades the entire screen (all 4 VDP palettes) out to black.
 *
 * This function utilizes SGDK's `VDP_fadeOutAll()` function, which smoothly
 * transitions all 64 VDP color entries (PAL0, PAL1, PAL2, and PAL3) to
 * black (color index 0x000). After initiating the fade, it calls `VDP_waitVSync()`
 * to pause execution until the fade effect has visually completed on the screen.
 *
 * It's generally expected that `store_current_palettes()` has been called
 * sometime before this function if a subsequent fade-in to the original palettes
 * is desired.
 *
 * @param speed_frames The duration of the fade-out effect, specified in video frames.
 *                     A value of 1 results in a very fast fade. Higher values create
 *                     a slower, more gradual fade. For example, on a 60Hz system,
 *                     a value of 30 would mean the fade takes 0.5 seconds.
 */
void transition_fade_out_to_black(u16 speed_frames) {
    // The `main.c` in this project calls `store_current_palettes()` explicitly
    // before calling this function. If this were a more general-purpose library
    // function intended for wider use, it might be safer to include a call to
    // `store_current_palettes()` here, or clearly document it as a prerequisite.
    // store_current_palettes();

    VDP_fadeOutAll(speed_frames); // SGDK function to fade all palettes to black
    VDP_waitVSync();              // Wait for the VSync to ensure the fade is complete
}

/**
 * @brief Fades the screen in from black to the previously stored palettes.
 *
 * This function uses SGDK's `VDP_fadeInAll()` to transition the screen colors
 * from black to a target palette. The target palette is constructed using the
 * color data previously saved by `store_current_palettes()` (i.e., `stored_palette0`
 * and `stored_palette1`).
 *
 * `VDP_fadeInAll` requires a single array of 64 colors representing the target state
 * for all four hardware palettes (PAL0-PAL3). This function prepares such an array:
 * - `stored_palette0` is copied to the first 16 entries.
 * - `stored_palette1` is copied to the next 16 entries.
 * - The entries for PAL2 and PAL3 (colors 32-63) are currently filled with black (0x000),
 *   assuming they are not used or should remain black after the fade-in. If PAL2/PAL3
 *   were also stored by `store_current_palettes()`, their data should be copied here instead.
 *
 * After initiating the fade, `VDP_waitVSync()` is called to ensure the fade-in
 * effect completes visually.
 *
 * @param speed_frames The duration of the fade-in effect, in video frames.
 *                     Similar to `transition_fade_out_to_black`, higher values mean
 *                     a slower transition.
 */
void transition_fade_in_from_black(u16 speed_frames) {
    // Assumes stored_palette0 and stored_palette1 contain the desired target palettes
    // for PAL0 and PAL1 respectively.

    // VDP_fadeInAll requires a single 64-color palette source array.
    // We construct this array here.
    static u16 full_target_palette[64]; // Declared static for safety, though could be local
                                       // if not used in an asynchronous context (which it isn't here).

    // Copy stored PAL0 data to the beginning of our full target palette
    memcpy(&full_target_palette[0],  stored_palette0, 16 * sizeof(u16));
    // Copy stored PAL1 data to the section for PAL1 in our full target palette
    memcpy(&full_target_palette[16], stored_palette1, 16 * sizeof(u16));

    // Assuming PAL2 and PAL3 are intended to be black or are unused after this fade-in.
    // Fill their corresponding sections in full_target_palette with black (0x000).
    // If PAL2 and PAL3 were also stored by `store_current_palettes()`,
    // their data should be copied here instead of this loop.
    for(int i = 32; i < 64; ++i) {
        full_target_palette[i] = 0x000; // Black
    }

    // Perform the fade-in operation using the constructed full palette
    VDP_fadeInAll(speed_frames, full_target_palette);
    VDP_waitVSync(); // Wait for VSync to ensure the fade is complete
}
