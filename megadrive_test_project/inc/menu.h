#ifndef MENU_H
#define MENU_H

#include <genesis.h>

#define MAX_MENU_ITEMS 8 // Was 7

// Initializes the menu system (e.g., sets default selection)
void menu_init();

// Handles input for menu navigation (Up, Down, Select)
// Returns an integer representing the selected test index, or -1 if no selection made.
// Or, it could directly trigger a state change via a function pointer or callback.
// For now, let's have it return an ID or handle state change internally.
void menu_handle_input();

// Draws the menu on screen
void menu_draw();

// Function to get the ID of the currently selected test,
// so main.c can decide which test to launch.
// Or menu_handle_input could change game state directly.
// Let's make menu_handle_input change a local variable, and add a getter.
s16 menu_get_selected_action_id(); // Returns action ID or -1
void menu_reset_action_id(); // Resets the action ID after it's been processed

#endif // MENU_H
