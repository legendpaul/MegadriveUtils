#ifndef DIALOGUE_ENGINE_H
#define DIALOGUE_ENGINE_H

#include <genesis.h>

#define MAX_DIALOGUE_LINES 4 // Max lines to display in the box at once
#define MAX_CHARS_PER_LINE 40 // Max characters per line (adjust based on box width)

typedef struct {
    const char* full_message;
    const char* current_message_ptr; // Pointer to the start of text for the current page
    char lines[MAX_DIALOGUE_LINES][MAX_CHARS_PER_LINE]; // Buffer for current page's lines
    u8 num_lines_on_current_page;
    u8 current_char_in_line; // For typewriter effect (future)
    u8 current_line_in_page; // For typewriter effect (future)
    // Add flags: is_active, needs_paging_indicator, etc.
    u8 is_active;
    u8 needs_paging_indicator; // Set if there's more text after current page
} DialogueState;


void dialogue_engine_init(); // Initializes the dialogue state
void dialogue_engine_draw_box(VDPPlane plane, u16 x, u16 y, u16 width, u16 height, const char* title);

// Starts displaying a new message. Parses first page.
void dialogue_engine_start_message(const char* message, u16 box_char_width, u16 box_max_lines);

// Draws the current page of text and the box.
void dialogue_engine_draw_current_page(VDPPlane plane, u16 box_tile_x, u16 box_tile_y, u16 box_width_tiles, u16 box_height_tiles, const char* title);

// Handles input to advance text (will be more relevant with paging)
// Returns TRUE if dialogue is still active, FALSE if finished.
u8 dialogue_engine_update(); // For now, might just return is_active

u8 dialogue_engine_is_active();

#endif // DIALOGUE_ENGINE_H
