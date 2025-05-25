#include "input_test.h"
#include "input.h"     // For new input system functions
#include <string.h>   // For strcat
#include <stdio.h>    // For sprintf

// Positions for displaying button states
#define INPUT_POS_X 5
#define INPUT_POS_Y 8

void input_test_init_display() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_setTextPalette(PAL0); // Assuming PAL0 has a usable white/light color
    VDP_drawText("Controller Input Test:", INPUT_POS_X, INPUT_POS_Y - 2);
    VDP_drawText("Press Start to Exit", INPUT_POS_X, INPUT_POS_Y + 10);
}

void input_test_update_display() {
    // Clear previous input lines to prevent overwriting issues
    VDP_clearText(INPUT_POS_X, INPUT_POS_Y, 30);      // Line 1 for D-Pad
    VDP_clearText(INPUT_POS_X, INPUT_POS_Y + 1, 30);  // Line 2 for Buttons
    VDP_clearText(INPUT_POS_X, INPUT_POS_Y + 3, 30);  // Line for Raw State

    char line1[30] = "";
    char line2[30] = "";

    if (input_is_held(BUTTON_UP)) strcat(line1, "UP "); else strcat(line1, "   ");
    if (input_is_held(BUTTON_DOWN)) strcat(line1, "DOWN "); else strcat(line1, "     ");
    if (input_is_held(BUTTON_LEFT)) strcat(line1, "LEFT "); else strcat(line1, "     ");
    if (input_is_held(BUTTON_RIGHT)) strcat(line1, "RIGHT "); // No need for extra spaces if it's the last

    VDP_drawText(line1, INPUT_POS_X, INPUT_POS_Y);

    if (input_is_held(BUTTON_A)) strcat(line2, "A "); else strcat(line2, "  ");
    if (input_is_held(BUTTON_B)) strcat(line2, "B "); else strcat(line2, "  ");
    if (input_is_held(BUTTON_C)) strcat(line2, "C "); else strcat(line2, "  ");
    if (input_is_held(BUTTON_START)) strcat(line2, "START "); // No need for extra spaces if last

    VDP_drawText(line2, INPUT_POS_X, INPUT_POS_Y + 1);

    // Display raw hex value using input_get_current_state() from input.c
    // This function was reported as added by the worker in the previous turn's subtask.
    char hex_val[20];
    sprintf(hex_val, "Raw State: %04X", input_get_current_state());
    VDP_drawText(hex_val, INPUT_POS_X, INPUT_POS_Y + 3);
}
