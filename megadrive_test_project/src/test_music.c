#include "test_music.h"
#include "music.h"       // Our new music module
#include "input.h"
#include "resources.h"   // For music_track_res_id
#include <string.h>      // For KLog or sprintf

static u8 music_playing = FALSE;
// Removed: static u16 prev_input_state = 0; (part of step 4)

void music_test_init() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_setTextPalette(PAL0);

    music_init_driver(); // Initialize XGM driver

    VDP_drawText("XGM Music Test", 10, 5);
    VDP_drawText("A: Play Music", 10, 8);
    VDP_drawText("B: Stop Music", 10, 9);
    VDP_drawText("Start: Exit to Menu", 10, 11);
    VDP_drawText("Note: Replace music_track.xgm", 2, 15);
    VDP_drawText("with a real XGM file!", 2, 16);

    music_playing = XGM_isPlaying(); // Check initial state
}

void music_test_update() {
    music_update_driver(); // Keep the XGM driver updated

    // Using new global input functions with edge detection
    // Removed: static u16 prev_input_state = 0;
    // Removed: u16 current_input_state = input_get_joy1_state();
    // Removed: u16 changed_input_state = current_input_state & ~prev_input_state;

    if (input_is_just_pressed(BUTTON_A)) { // Was changed_input_state & BUTTON_A
        music_start(music_track_res_id); // music_track_res_id from resources.h
        music_playing = TRUE;
    }
    if (input_is_just_pressed(BUTTON_B)) { // Was changed_input_state & BUTTON_B
        music_stop();
        music_playing = FALSE;
    }
    
    // Removed: prev_input_state = current_input_state;

    // Display music status
    VDP_clearText(10, 13, 20); // Clear previous status
    if (music_playing) {
        VDP_drawText("Music Playing", 10, 13);
    } else {
        VDP_drawText("Music Stopped", 10, 13);
    }
    
    // Check if music stopped by itself (e.g. end of track)
    // This is important because XGM_isPlaying() will return FALSE then.
    if (music_playing && !XGM_isPlaying()) {
        music_playing = FALSE;
    }
}

void music_test_on_exit() {
    music_stop(); // Ensure music is stopped when exiting the test
    // XGM_end(); // Optional: if you want to completely shut down XGM. For now, stop is enough.
}
