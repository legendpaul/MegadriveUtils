/**
 * @file input.h
 * @brief Header file for controller input handling.
 *
 * This module provides an abstraction layer for reading input from Sega Mega Drive
 * controllers. It supports initializing the input system, updating input state
 * each frame, and helper functions to check the status of specific buttons
 * or D-Pad directions.
 */
#ifndef INPUT_H
#define INPUT_H

#include <genesis.h> // SGDK general header

/**
 * @brief Initializes the input system.
 *
 * This function should be called once at the start of the application.
 * Currently, it's a placeholder as SGDK's `SGDK_init()` (called in `main.c`)
 * already initializes the underlying JOY (joystick/joypad) manager.
 */
void input_init();

/**
 * @brief Updates the input state for the current frame.
 *
 * This function should be called once per game loop, typically at the beginning.
 * It reads the current state of Joypad 1 and stores it internally for use by
 * other input checking functions during that frame.
 */
void input_update();

/**
 * @brief Retrieves the raw state of Joypad 1.
 *
 * This function returns the raw bitmask representing the state of all buttons
 * and D-Pad directions for Joypad 1. This can be useful for more complex input
 * processing or debugging.
 *
 * @return u16 A bitmask where each bit corresponds to a button or direction.
 *             Common masks include `BUTTON_A`, `BUTTON_B`, `BUTTON_C`, `BUTTON_START`,
 *             `BUTTON_UP`, `BUTTON_DOWN`, `BUTTON_LEFT`, `BUTTON_RIGHT`.
 */
u16 input_get_joy1_state();

/**
 * @brief Checks if a specific button (or buttons) is currently pressed on Joypad 1.
 *
 * @param button_mask A bitmask representing the button(s) to check (e.g., `BUTTON_A`,
 *                    `BUTTON_START`, or `BUTTON_A | BUTTON_B`).
 * @return u8 Returns 1 if any of the specified buttons in the mask are pressed, 0 otherwise.
 */
u8 input_is_button_pressed(u16 button_mask);

/**
 * @brief Checks if the D-Pad Left is currently pressed on Joypad 1.
 * @return u8 Returns 1 if D-Pad Left is pressed, 0 otherwise.
 */
u8 input_is_dpad_left_pressed();

/**
 * @brief Checks if the D-Pad Right is currently pressed on Joypad 1.
 * @return u8 Returns 1 if D-Pad Right is pressed, 0 otherwise.
 */
u8 input_is_dpad_right_pressed();

/**
 * @brief Checks if the D-Pad Up is currently pressed on Joypad 1.
 * @return u8 Returns 1 if D-Pad Up is pressed, 0 otherwise.
 */
u8 input_is_dpad_up_pressed();

/**
 * @brief Checks if the D-Pad Down is currently pressed on Joypad 1.
 * @return u8 Returns 1 if D-Pad Down is pressed, 0 otherwise.
 */
u8 input_is_dpad_down_pressed();

#endif // INPUT_H
