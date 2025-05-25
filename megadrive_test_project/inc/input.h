#ifndef INPUT_H
#define INPUT_H

#include <genesis.h>

void input_init();    // Call once at startup
void input_update();  // Call once per frame in the main loop

// Helper functions to check specific buttons/directions on Joypad 1
u8 input_is_held(u16 button_mask); // True if button is currently down
u8 input_is_just_pressed(u16 button_mask); // True if button was just pressed this frame
u8 input_is_just_released(u16 button_mask); // True if button was just released this frame
    u16 input_get_current_state(); // Getter for the raw current state, useful for display/debug

// Keep D-Pad specific helpers if they are still convenient, but they should use input_is_held internally.
// Or, users can call input_is_held(BUTTON_LEFT), etc.
// Let's simplify and remove the specific D-Pad helpers from here, promoting use of input_is_held(BUTTON_...).
// u8 input_is_dpad_left_pressed();  // REMOVE or update to use input_is_held
// u8 input_is_dpad_right_pressed(); // REMOVE or update
// u8 input_is_dpad_up_pressed();    // REMOVE or update
// u8 input_is_dpad_down_pressed();  // REMOVE or update

#endif // INPUT_H
