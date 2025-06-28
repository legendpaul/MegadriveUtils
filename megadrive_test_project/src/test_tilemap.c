#include "test_tilemap.h"
#include "graphics.h" // For load_simple_tileset(), display_simple_tilemap()
#include "input.h"    // For input_is_just_pressed() and BUTTON_START (if exit handled here)
// #include "main.h" // If calling a main_return_to_menu() function directly

void test_tilemap_init() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    load_simple_tileset();    // From graphics.c
    display_simple_tilemap(); // From graphics.c

    VDP_drawText("Tilemap Test - Press Start to Exit", 2, 26);
}

void test_tilemap_update() {
    // This function is intentionally empty as the tilemap display is static.
    // The exit condition (Start button press) is handled in main.c's game loop
    // for this particular test state.
    // Alternatively, if this function were to handle its own exit signal:
    // if (input_is_just_pressed(BUTTON_START)) {
    //    // Signal main to return to menu, e.g., by setting a flag or calling a function
    // }
}

void test_tilemap_on_exit() {
    // Specific cleanup for the tilemap test before returning to menu.
    // VDP_clearPlane(BG_A, TRUE) is called by main.c's return_to_menu(),
    // so it's not strictly needed here unless there are other specific resources.
    // For now, this can be empty.
}
