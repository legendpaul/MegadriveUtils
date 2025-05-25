#include "test_fades.h"
#include "genesis.h"     // For VDP_ functions, SYS_getTime, etc.
#include "transitions.h" // For store_current_palettes, transition_fade_out/in
#include "input.h"       // For input_is_just_pressed (though exit is handled in main.c for this test)

// --- Variables and Enum for Fade Test (Moved from main.c) ---
static u16 fade_test_palette[16]; // To store a custom palette for this test
typedef enum {
    FTS_INIT, FTS_SHOW_INITIAL, FTS_FADING_OUT, FTS_WAIT_BLACK, FTS_FADING_IN, FTS_DONE
} FadeTestSubState;
static FadeTestSubState fade_test_current_sub_state;
static u32 fade_test_timer;

void test_fades_init() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    // Create a simple, bright palette for PAL0 for this test
    fade_test_palette[0] = RGB24_TO_VDPCOLOR(0x222222); // Dark grey background
    fade_test_palette[1] = RGB24_TO_VDPCOLOR(0xFF0000); // Red
    fade_test_palette[2] = RGB24_TO_VDPCOLOR(0x00FF00); // Green
    fade_test_palette[3] = RGB24_TO_VDPCOLOR(0x0000FF); // Blue
    for(int i=4; i<16; ++i) fade_test_palette[i] = fade_test_palette[0]; // Fill rest
    VDP_setPalette(PAL0, fade_test_palette);

    // Draw some colored text/elements to showcase the fade
    VDP_setTextPalette(PAL0);
    VDP_drawText("Red", 12, 12);
    VDP_drawText("Green", 12, 14);
    VDP_drawText("Blue", 12, 16);
    VDP_drawText("Watch the Fades!", 10, 5);

    store_current_palettes(); // Store PAL0 (custom bright one) and PAL1 (likely default/black)

    fade_test_current_sub_state = FTS_INIT; // Initialize the sub-state for the fade test
    // current_game_state = STATE_TEST_FADES; // This will be set in main.c after calling this init
}

void test_fades_update() {
    // This function manages the sequence of the fade test
    switch (fade_test_current_sub_state) {
        case FTS_INIT:
            VDP_drawText("Fade Test. Will start in 2s.", 5, 20);
            fade_test_timer = SYS_getTime();
            fade_test_current_sub_state = FTS_SHOW_INITIAL;
            break;
        case FTS_SHOW_INITIAL:
            if (SYS_getTime() - fade_test_timer > (2 * SGDK_TIMER_NORMAL_DIV)) {
                VDP_clearText(5, 20, 30);
                VDP_drawText("Fading Out...", 10, 20);
                transition_fade_out_to_black(60); // 1-second fade out
                fade_test_timer = SYS_getTime();
                fade_test_current_sub_state = FTS_WAIT_BLACK;
            }
            break;
        case FTS_WAIT_BLACK:
            if (SYS_getTime() - fade_test_timer > (1 * SGDK_TIMER_NORMAL_DIV)) {
                VDP_clearText(10, 20, 30);
                VDP_drawText("Fading In...", 10, 20);
                transition_fade_in_from_black(60); // 1-second fade in
                fade_test_current_sub_state = FTS_DONE;
            }
            break;
        case FTS_DONE:
            VDP_clearText(10, 20, 30);
            VDP_drawText("Fade Test Complete. Press Start.", 2, 26);
            // No automatic transition from here, user exits with Start (handled in main.c)
            break;
    }
    // The main loop's switch handles Start press to exit to menu
}

void test_fades_on_exit() {
    // This function can be empty for now.
    // Specific cleanup for the fades test before returning to menu.
    // VDP_clearPlane(BG_A, TRUE) and palette reset (via menu_init) are handled by main.c's return_to_menu().
}
