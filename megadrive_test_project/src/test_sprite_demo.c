#include "test_sprite_demo.h"
#include "graphics.h"    // For setup_sprites() and update_sprites_example()
#include "sound.h"       // For init_sound_system() (if setup_sprites doesn't call it)
#include "input.h"       // For input_is_just_pressed() and BUTTON_START
#include "transitions.h" // For store_current_palettes(), transition_fade_out_to_black(), transition_fade_in_from_black()
// #include "main.h" // Not needed if main.c handles the exit trigger

void test_sprite_demo_init() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_setPaletteColor(0, RGB24_TO_VDPCOLOR(0x000040)); // Background color for sprite demo
    
    // setup_sprites() from graphics.c initializes sprites, loads palettes,
    // and previously also called init_sound_system() and input_init().
    // We assume init_sound_system() and input_init() are called globally in main.c now.
    setup_sprites(); 
    
    store_current_palettes(); // Store palettes for the fade effect
    transition_fade_out_to_black(15); // Quick fade out before starting
    transition_fade_in_from_black(15);  // Quick fade in to the demo

    VDP_drawText("Sprite Demo - Start to Exit", 2, 26);
}

void test_sprite_demo_update() {
    // update_sprites_example() from graphics.c contains the main logic:
    // - D-Pad movement for player_x, player_y
    // - Boundary checks
    // - BUTTON_A check to play sound
    // - Player animation update
    // - SPR_update()
    update_sprites_example();

    // Exit condition (Start button press) will be checked in main.c's game loop
    // as per the simplified instructions.
    // If this function were to signal exit:
    // if (input_is_just_pressed(BUTTON_START)) {
    //    return TRUE; // or call a state change function
    // }
    // return FALSE;
}

void test_sprite_demo_on_exit() {
    // Specific cleanup for the sprite demo before returning to menu.
    // SPR_end() is called by main.c's return_to_menu(), so it's not strictly needed here
    // unless there are other sprite-demo-specific resources to release that SPR_end doesn't cover.
    // For now, this can be empty.
    // Example: SPR_end();
}
