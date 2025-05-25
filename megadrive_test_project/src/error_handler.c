#include "error_handler.h"
#include <string.h> // For strlen, strcpy, strcat
#include <stdio.h>  // For sprintf (not used if _int_to_string_decimal is used)

// Simple int to string for line numbers (decimal, up to 5 digits for u16)
// Buffer should be at least 6 chars long (5 digits + null terminator).
static void _int_to_string_decimal(u16 value, char* buffer) {
    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    char temp[6];
    int i = 0;
    int j = 0;

    while (value > 0) {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }
    
    // Reverse the string
    for (j = 0; j < i; ++j) {
        buffer[j] = temp[i - 1 - j];
    }
    buffer[j] = '\0';
}


void error_handler_display_error(const char* module_name, 
                                 const char* function_name, 
                                 u16 line_number, 
                                 const char* error_message) {
    // Halt system and clear screen
    SYS_disableInts();
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    SPR_end(); // Clear sprites

    // Set a distinct error palette
    VDP_setPaletteColor(0, RGB24_TO_VDPCOLOR(0x000000)); // Black background
    VDP_setPaletteColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF)); // White text 
    VDP_setTextPalette(PAL0); // Ensure text uses PAL0

    u16 y_pos = 2; // Starting Y position for text

    VDP_drawText("FATAL ERROR!", 2, y_pos); 
    y_pos += 2;

    char line_buf[40]; // Buffer for formatting lines with prefixes

    strcpy(line_buf, "MODULE: "); strcat(line_buf, module_name);
    VDP_drawText(line_buf, 2, y_pos++);

    strcpy(line_buf, "FUNC: "); strcat(line_buf, function_name);
    VDP_drawText(line_buf, 2, y_pos++);

    char line_num_str[6];
    _int_to_string_decimal(line_number, line_num_str);
    strcpy(line_buf, "LINE: "); strcat(line_buf, line_num_str);
    VDP_drawText(line_buf, 2, y_pos++);
    y_pos++; // Extra space

    strcpy(line_buf, "MSG: "); 
    // Ensure message fits or truncate
    strncat(line_buf, error_message, sizeof(line_buf) - strlen(line_buf) - 1);
    VDP_drawText(line_buf, 2, y_pos++);

    // Halt
    while(1) {
        SYS_doVBlankProcess(); // Keep VDP alive
    }
}
