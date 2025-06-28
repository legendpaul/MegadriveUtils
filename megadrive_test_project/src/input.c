#include "input.h"
#include "error_handler.h" // For error reporting

typedef struct {
    u16 current;
    u16 prev;
} InputState;

static InputState joy1_input_state;
static u8 is_input_initialized = FALSE; // Initialization flag

// Module name for error reporting
#define MODULE_NAME_INPUT "input"

void input_init() {
    // JOY_init() is called by SGDK_init().
    joy1_input_state.current = 0;
    joy1_input_state.prev = 0;
    is_input_initialized = TRUE;
    // KLog("Input system initialized."); // Optional: KLog for debug on emulator
}

void input_update() {
    if (!is_input_initialized) {
        error_handler_display_error(MODULE_NAME_INPUT, __func__, __LINE__, "Not initialized!");
        // __func__ gives current function name, __LINE__ gives line number
        return; // Should halt in error_handler, but good practice
    }
    joy1_input_state.prev = joy1_input_state.current;
    joy1_input_state.current = JOY_readJoypad(JOY_1);
}

u8 input_is_held(u16 button_mask) {
    if (!is_input_initialized) {
        error_handler_display_error(MODULE_NAME_INPUT, __func__, __LINE__, "Not initialized!");
        return FALSE; 
    }
    return (joy1_input_state.current & button_mask) ? 1 : 0;
}

u8 input_is_just_pressed(u16 button_mask) {
    if (!is_input_initialized) {
        error_handler_display_error(MODULE_NAME_INPUT, __func__, __LINE__, "Not initialized!");
        return FALSE;
    }
    return ((joy1_input_state.current & button_mask) && 
            !(joy1_input_state.prev & button_mask)) ? 1 : 0;
}

u8 input_is_just_released(u16 button_mask) {
    if (!is_input_initialized) {
        error_handler_display_error(MODULE_NAME_INPUT, __func__, __LINE__, "Not initialized!");
        return FALSE;
    }
    return (!(joy1_input_state.current & button_mask) && 
            (joy1_input_state.prev & button_mask)) ? 1 : 0;
}

u16 input_get_current_state() {
    if (!is_input_initialized) {
        error_handler_display_error(MODULE_NAME_INPUT, __func__, __LINE__, "Not initialized!");
        return 0; 
    }
    return joy1_input_state.current;
}
