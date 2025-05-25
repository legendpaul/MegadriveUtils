#include "test_dialogue.h"
#include "dialogue_engine.h" // Our new dialogue engine
#include "input.h"
#include <genesis.h>

#define BOX_X 5
#define BOX_Y 18 // Position box lower on screen
#define BOX_WIDTH 30
#define BOX_HEIGHT 6

void dialogue_test_init() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_setTextPalette(PAL0); // Ensure text uses a known palette (e.g., PAL0 color 15 for white)

    dialogue_engine_init(); // Call if it does anything (currently empty)

    // Draw the box
    dialogue_engine_draw_box(BG_A, BOX_X, BOX_Y, BOX_WIDTH, BOX_HEIGHT, "Info");

    // Draw text inside
    const char* test_message = "Hello Dialogue Box!";
    dialogue_engine_draw_text_line_inside_box(BG_A, test_message, BOX_X + 1, BOX_Y + 1);
    
    const char* test_message2 = "This is a simple test.";
    dialogue_engine_draw_text_line_inside_box(BG_A, test_message2, BOX_X + 1, BOX_Y + 2);


    VDP_drawText("Dialogue Test (Simple) - Start to Exit", 2, 2);
}

void dialogue_test_update() {
    // Static display for now
    // Exit condition (Start button press) is handled in main.c's game loop
}

void dialogue_test_on_exit() {
    // Nothing specific for now
    // VDP_clearPlane(BG_A, TRUE) is handled by main.c's return_to_menu()
}
