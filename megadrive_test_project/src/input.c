/**
 * @file input.c
 * @brief Implements controller input handling.
 *
 * This module provides functions to initialize the input system, read controller
 * states each frame, and helper functions to query the status of specific buttons
 * or D-Pad directions. It currently focuses on Joypad 1.
 */
#include "input.h"

/**
 * @brief Stores the current state of Joypad 1 for the active frame.
 *
 * This variable is updated by `input_update()` each frame. It holds a bitmask
 * where each bit represents the state (pressed/released) of a button or D-Pad direction.
 * SGDK constants like `BUTTON_A`, `BUTTON_LEFT`, etc., can be used to check specific bits.
 */
static u16 joy1_state = 0;

/**
 * @brief Initializes the input system.
 *
 * This function is intended for any one-time setup required for input handling.
 * Currently, SGDK's main initialization `SGDK_init()` (called in `main.c`)
 * also initializes the JOY (joystick/joypad) manager, so this function
 * doesn't need to perform additional actions for basic joypad reading.
 */
void input_init() {
    // JOY_init() is called by SGDK_init(), so nothing specific needed here for now
    // for basic Joypad 1 reading.
}

/**
 * @brief Updates the internal input state for Joypad 1.
 *
 * This function should be called once per game loop, typically at the beginning,
 * before any game logic that depends on input. It reads the current hardware state
 * of Joypad 1 using `JOY_readJoypad(JOY_1)` and stores it in the static variable
 * `joy1_state`. This stored state is then used by other helper functions in this
 * module to check for specific button presses within the same frame.
 */
void input_update() {
    // Read the state of Joypad 1 and store it
    joy1_state = JOY_readJoypad(JOY_1);
}

/**
 * @brief Retrieves the raw state of Joypad 1 for the current frame.
 *
 * This function returns the `joy1_state` variable, which was updated by the last
 * call to `input_update()`. The returned value is a bitmask representing the
 * state of all buttons and D-Pad directions.
 *
 * @return u16 The raw bitmask state of Joypad 1.
 */
u16 input_get_joy1_state() {
    return joy1_state;
}

/**
 * @brief Checks if a specific button (or buttons) is currently pressed on Joypad 1.
 *
 * This function checks if any of the buttons corresponding to the bits set in
 * `button_mask` are currently pressed. The check is performed against the
 * `joy1_state` captured by `input_update()` for the current frame.
 *
 * @param button_mask A bitmask representing the button(s) to check.
 *                    For example, `BUTTON_A`, `BUTTON_START`, or `BUTTON_A | BUTTON_B`.
 * @return u8 Returns 1 (true) if any of the specified buttons are pressed, 0 (false) otherwise.
 */
u8 input_is_button_pressed(u16 button_mask) {
    // Check if the specified button(s) in button_mask is set in joy1_state
    return (joy1_state & button_mask) ? 1 : 0;
}

/**
 * @brief Checks if the D-Pad Left is currently pressed on Joypad 1.
 * @return u8 Returns 1 if D-Pad Left is pressed, 0 otherwise.
 */
u8 input_is_dpad_left_pressed() {
    return (joy1_state & BUTTON_LEFT) ? 1 : 0;
}

/**
 * @brief Checks if the D-Pad Right is currently pressed on Joypad 1.
 * @return u8 Returns 1 if D-Pad Right is pressed, 0 otherwise.
 */
u8 input_is_dpad_right_pressed() {
    return (joy1_state & BUTTON_RIGHT) ? 1 : 0;
}

/**
 * @brief Checks if the D-Pad Up is currently pressed on Joypad 1.
 * @return u8 Returns 1 if D-Pad Up is pressed, 0 otherwise.
 */
u8 input_is_dpad_up_pressed() {
    return (joy1_state & BUTTON_UP) ? 1 : 0;
}

/**
 * @brief Checks if the D-Pad Down is currently pressed on Joypad 1.
 * @return u8 Returns 1 if D-Pad Down is pressed, 0 otherwise.
 */
u8 input_is_dpad_down_pressed() {
    return (joy1_state & BUTTON_DOWN) ? 1 : 0;
}
