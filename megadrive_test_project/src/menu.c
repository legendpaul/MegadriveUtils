#include "menu.h"
#include "input.h"   // For input_get_joy1_state()
#include <string.h>  // For strcpy, strcat

// Define menu items
static const char* menu_items[MAX_MENU_ITEMS] = {
    "1. Show Sprite Demo",
    "2. Show Tilemap",
    "3. Test Fades",
    "4. Test Inputs",
    "5. Scrolling Demo",
    "6. XGM Music Test" // New item
};

static s16 current_selection = 0;
static s16 selected_action_id = -1; // -1 means no action, otherwise use current_selection

// For highlighting
#define MENU_CURSOR "> "
#define MENU_NOCURSOR "  "
#define MENU_START_X 5  // X position for menu text (in tiles)
#define MENU_START_Y 8  // Y position for the first menu item (in tiles)

// Removed: static u16 prev_joy_state = 0; 

void menu_init() {
    current_selection = 0;
    selected_action_id = -1;
    // Removed: prev_joy_state = 0; 

    // Clear VDP planes and set background color for menu
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_setPaletteColor(0, RGB24_TO_VDPCOLOR(0x000022)); // Darker blue for menu background

    // Set text palette (e.g., PAL1). Ensure PAL1 has a light color at a suitable index.
    // If default SGDK font uses color index 15 from the palette:
    // VDP_setPaletteColor(15 + 16, RGB24_TO_VDPCOLOR(0xFFFFFF)); // White on PAL1, index 15
    // VDP_setTextPalette(PAL1);
    // For simplicity, if PAL0 color 15 is white, default PAL0 might be okay.
    // Let's assume default PAL0 (index 15) is white for now.
    VDP_setTextPalette(PAL0); 

    menu_draw(); // Initial draw of the menu
}

void menu_handle_input() {
    if (selected_action_id != -1) {
        // An action has been selected and not yet processed by main loop.
        // Do not process further menu input until it's reset.
        return;
    }

    // input_update() is called once per frame in main.c's game loop.
    // Now we use the global input module's edge detection functions.

    if (input_is_just_pressed(BUTTON_UP)) {
        current_selection--;
        if (current_selection < 0) {
            current_selection = MAX_MENU_ITEMS - 1;
        }
        menu_draw(); // Redraw menu to show new selection
    } else if (input_is_just_pressed(BUTTON_DOWN)) {
        current_selection++;
        if (current_selection >= MAX_MENU_ITEMS) {
            current_selection = 0;
        }
        menu_draw(); // Redraw menu
    } else if (input_is_just_pressed(BUTTON_START | BUTTON_A)) { // Check for either button
        selected_action_id = current_selection; // Store index of selected item (0 to MAX_MENU_ITEMS-1)
        // Sound for selection could be played here, e.g. play_sfx_ping();
        // This will be handled by main.c based on the action ID.
    }

    // Removed: prev_joy_state = current_joy_state; 
}

void menu_draw() {
    // Clear previous menu text.
    // VDP_clearText(startX, startY, width) clears a region in characters (tiles).
    // Max item length is around 20-25 chars. Cursor adds 2.
    for (s16 i = 0; i < MAX_MENU_ITEMS; i++) {
        VDP_clearText(MENU_START_X, MENU_START_Y + i, 30); 
    }
    VDP_clearText(MENU_START_X, MENU_START_Y + MAX_MENU_ITEMS + 2, 35); // Clear help text line

    char buffer[40]; // Buffer for constructing menu lines

    for (s16 i = 0; i < MAX_MENU_ITEMS; i++) {
        if (i == current_selection) {
            strcpy(buffer, MENU_CURSOR);
        } else {
            strcpy(buffer, MENU_NOCURSOR);
        }
        strcat(buffer, menu_items[i]);
        // Draw text on BG_A. Ensure BG_A is configured and visible.
        VDP_drawText(buffer, MENU_START_X, MENU_START_Y + i);
    }

    VDP_drawText("Use D-Pad Up/Down, Start/A to select.", MENU_START_X, MENU_START_Y + MAX_MENU_ITEMS + 2);
}

s16 menu_get_selected_action_id() {
    return selected_action_id;
}

void menu_reset_action_id() {
    selected_action_id = -1;
}
