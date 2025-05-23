#include "input_test.h"
#include <string.h> // For strcpy, strcat, sprintf

// Positions for displaying button states
#define INPUT_POS_X 5
#define INPUT_POS_Y 8

void input_test_init_display() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_setTextPalette(PAL0); // Assuming PAL0 has a usable white/light color at index 15
    VDP_drawText("Controller Input Test:", INPUT_POS_X, INPUT_POS_Y - 2);
    VDP_drawText("Press Start to Exit", INPUT_POS_X, INPUT_POS_Y + 10);
}

void input_test_update_display(u16 joy_state) {
    // Clear previous input lines to prevent text overlap
    VDP_clearText(INPUT_POS_X, INPUT_POS_Y, 30);      // Line for D-Pad
    VDP_clearText(INPUT_POS_X, INPUT_POS_Y + 1, 30);  // Line for Buttons
    VDP_clearText(INPUT_POS_X, INPUT_POS_Y + 3, 30);  // Line for Hex Value

    char line1[30] = ""; // Buffer for D-Pad states
    char line2[30] = ""; // Buffer for Button states

    if (joy_state & BUTTON_UP) strcat(line1, "UP ");
    else strcat(line1, "   "); // Keep spacing consistent
    if (joy_state & BUTTON_DOWN) strcat(line1, "DOWN ");
    else strcat(line1, "     ");
    if (joy_state & BUTTON_LEFT) strcat(line1, "LEFT ");
    else strcat(line1, "     ");
    if (joy_state & BUTTON_RIGHT) strcat(line1, "RIGHT ");
    else strcat(line1, "      ");

    VDP_drawText(line1, INPUT_POS_X, INPUT_POS_Y);

    if (joy_state & BUTTON_A) strcat(line2, "A ");
    else strcat(line2, "  ");
    if (joy_state & BUTTON_B) strcat(line2, "B ");
    else strcat(line2, "  ");
    if (joy_state & BUTTON_C) strcat(line2, "C ");
    else strcat(line2, "  ");
    if (joy_state & BUTTON_START) strcat(line2, "START ");
    else strcat(line2, "      ");

    VDP_drawText(line2, INPUT_POS_X, INPUT_POS_Y + 1);

    // Display raw hex value of joy_state for debugging
    char hex_val[15]; // Increased size slightly for "State: XXXX"
    // Using sgdk_sprintf for potentially lighter weight alternative if available,
    // otherwise standard sprintf. For SGDK, direct int to hex is often preferred.
    // For now, let's stick to standard sprintf as per prompt.
    sprintf(hex_val, "State: %04X", joy_state);
    VDP_drawText(hex_val, INPUT_POS_X, INPUT_POS_Y + 3);
}
