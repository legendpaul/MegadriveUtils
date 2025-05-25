#include "dialogue_engine.h"
#include "input.h" 
#include <string.h> 

static DialogueState current_dialogue;

// These constants are used by dialogue_engine_update to pass to _dialogue_engine_prepare_page
// They should ideally match how the box is drawn in test_dialogue.c
// For now, hardcoding reasonable defaults. A better system would pass these from the test module.
// These are effectively the *content area* of the box, not the full box size.
#define DIALOGUE_BOX_CONTENT_CHAR_WIDTH (30 - 2) // Example: 30 tile wide box - 2 for borders
#define DIALOGUE_BOX_CONTENT_MAX_LINES (MAX_DIALOGUE_LINES)  // Use the define from header directly


#define FONT_CHAR_SPACE (VDP_getFontTileInd() + (' ' - ' '))
#define FONT_CHAR_HLINE (VDP_getFontTileInd() + ('-' - ' '))
#define FONT_CHAR_VLINE (VDP_getFontTileInd() + ('|' - ' '))
#define FONT_CHAR_TL    (VDP_getFontTileInd() + ('+' - ' '))
#define FONT_CHAR_TR    (VDP_getFontTileInd() + ('+' - ' '))
#define FONT_CHAR_BL    (VDP_getFontTileInd() + ('+' - ' '))
#define FONT_CHAR_BR    (VDP_getFontTileInd() + ('+' - ' '))
#define BOX_ATTR TILE_ATTR(PAL0, FALSE, FALSE, FALSE)

void dialogue_engine_init() {
    memset(&current_dialogue, 0, sizeof(DialogueState));
    current_dialogue.is_active = FALSE;
}

void dialogue_engine_draw_box(VDPPlane plane, u16 x, u16 y, u16 width, u16 height, const char* title) {
    if (width < 2 || height < 2) return;
    VDP_setTileMapXY(plane, BOX_ATTR | FONT_CHAR_TL, x, y);
    VDP_setTileMapXY(plane, BOX_ATTR | FONT_CHAR_TR, x + width - 1, y);
    VDP_setTileMapXY(plane, BOX_ATTR | FONT_CHAR_BL, x, y + height - 1);
    VDP_setTileMapXY(plane, BOX_ATTR | FONT_CHAR_BR, x + width - 1, y + height - 1);
    for (u16 i = 1; i < width - 1; ++i) {
        VDP_setTileMapXY(plane, BOX_ATTR | FONT_CHAR_HLINE, x + i, y);
        VDP_setTileMapXY(plane, BOX_ATTR | FONT_CHAR_HLINE, x + i, y + height - 1);
    }
    for (u16 i = 1; i < height - 1; ++i) {
        VDP_setTileMapXY(plane, BOX_ATTR | FONT_CHAR_VLINE, x, y + i);
        VDP_setTileMapXY(plane, BOX_ATTR | FONT_CHAR_VLINE, x + width - 1, y + i);
    }
    for (u16 iy = 1; iy < height - 1; ++iy) {
        for (u16 ix = 1; ix < width - 1; ++ix) {
            VDP_setTileMapXY(plane, BOX_ATTR | FONT_CHAR_SPACE, x + ix, y + iy);
        }
    }
    if (title != NULL) {
        int title_len = strlen(title);
        int title_start_x = x + (width - title_len) / 2;
        if (title_start_x < x + 1) title_start_x = x + 1;
        // Ensure title does not overflow box width
        if (title_len > 0 && title_len <= (width - 2)) { 
            VDP_drawTextEx(plane, title, title_start_x, y, BOX_ATTR, FALSE); 
        } else if (title_len > (width - 2) && (width - 2 > 0)) { // Check if box is wide enough for any title
             // If title is too long, draw a truncated version (very basic truncation)
            char truncated_title[MAX_CHARS_PER_LINE]; 
            strncpy(truncated_title, title, width - 3); // width-2 for content, -1 for ellipsis if added
            truncated_title[width - 3] = '\0'; 
            // strcat(truncated_title, "."); // Example: Add ellipsis, ensure buffer is large enough
            VDP_drawTextEx(plane, truncated_title, x + 1, y, BOX_ATTR, FALSE);
        }
    }
}

// Internal helper: Parses text and fills line buffers for the current page
static void _dialogue_engine_prepare_page(u16 box_char_width, u16 box_max_lines) {
    // Clear previous page lines
    for(u8 i=0; i < MAX_DIALOGUE_LINES; ++i) {
        current_dialogue.lines[i][0] = '\0';
    }
    current_dialogue.num_lines_on_current_page = 0;

    if (!current_dialogue.current_message_ptr || *current_dialogue.current_message_ptr == '\0') {
        current_dialogue.is_active = FALSE; // No more text at all
        current_dialogue.needs_paging_indicator = FALSE;
        return;
    }

    const char* text_ptr = current_dialogue.current_message_ptr;

    for (u8 line_idx = 0; line_idx < box_max_lines && line_idx < MAX_DIALOGUE_LINES; ++line_idx) {
        // Skip leading spaces for a new line (but not for the very first char of a page segment)
        if (text_ptr != current_dialogue.current_message_ptr || line_idx > 0) {
            while(*text_ptr == ' ' && *text_ptr != '\0') { // Ensure not to skip null terminator
                text_ptr++;
            }
        }
        
        if (*text_ptr == '\0') { // Check after skipping spaces
             current_dialogue.needs_paging_indicator = FALSE; 
             break;
        }

        int len_to_copy = 0;
        const char* line_end_ptr = text_ptr; 
        const char* last_space = NULL;

        for (int char_idx = 0; char_idx < box_char_width; ++char_idx) {
            if (text_ptr[char_idx] == '\0') {
                line_end_ptr = &text_ptr[char_idx];
                len_to_copy = char_idx;
                break; 
            }
            if (text_ptr[char_idx] == '\n') {
                line_end_ptr = &text_ptr[char_idx];
                len_to_copy = char_idx;
                break;
            }
            if (text_ptr[char_idx] == ' ') {
                last_space = &text_ptr[char_idx];
            }
            line_end_ptr = &text_ptr[char_idx + 1]; 
            len_to_copy = char_idx + 1;
        }
        
        // Word wrapping: if line is full and next char isn't a natural break
        if (len_to_copy == box_char_width && *line_end_ptr != '\0' && *line_end_ptr != '\n' && *line_end_ptr != ' ') {
            if (last_space != NULL && (last_space > text_ptr && (last_space - text_ptr) < box_char_width) ) { 
                line_end_ptr = last_space; // Point to the space
                len_to_copy = last_space - text_ptr; // Length up to the space
            }
            // If no space found, hard break (len_to_copy remains box_char_width, line_end_ptr is after char_width chars)
        }
        
        strncpy(current_dialogue.lines[line_idx], text_ptr, len_to_copy);
        current_dialogue.lines[line_idx][len_to_copy] = '\0';
        current_dialogue.num_lines_on_current_page++;

        text_ptr = line_end_ptr; // Advance master pointer
        if (*text_ptr == '\n' || *text_ptr == ' ') { // Advance past the break character itself for next segment
            text_ptr++;
        }
        
        if (*text_ptr == '\0') { // If we've reached the end of the full message
            current_dialogue.needs_paging_indicator = FALSE;
            break; 
        }
    }
    current_dialogue.current_message_ptr = text_ptr; // Update pointer for next page
    current_dialogue.needs_paging_indicator = (*current_dialogue.current_message_ptr != '\0');
}

void dialogue_engine_start_message(const char* message, u16 box_char_width, u16 box_max_lines) {
    dialogue_engine_init(); 
    current_dialogue.full_message = message;
    current_dialogue.current_message_ptr = message;
    current_dialogue.is_active = TRUE;

    _dialogue_engine_prepare_page(box_char_width, box_max_lines);
    
    // If the message was empty or only spaces, _prepare_page might result in 0 lines
    if (current_dialogue.num_lines_on_current_page == 0 && !current_dialogue.needs_paging_indicator) {
        current_dialogue.is_active = FALSE; 
    }
}
    
void dialogue_engine_draw_current_page(VDPPlane plane, u16 box_tile_x, u16 box_tile_y, u16 box_width_tiles, u16 box_height_tiles, const char* title) {
    // Only draw if dialogue is active OR if it just became inactive but still has lines (last page was shown)
    if (!current_dialogue.is_active && current_dialogue.num_lines_on_current_page == 0) return;

    dialogue_engine_draw_box(plane, box_tile_x, box_tile_y, box_width_tiles, box_height_tiles, title);

    for (u8 i = 0; i < current_dialogue.num_lines_on_current_page; ++i) {
        VDP_drawTextEx(plane, current_dialogue.lines[i], box_tile_x + 1, box_tile_y + 1 + i, BOX_ATTR, FALSE);
    }
    // Paging indicator drawing will be added later
}

u8 dialogue_engine_update() {
    if (!current_dialogue.is_active) return FALSE;

    if (current_dialogue.needs_paging_indicator) {
        if (input_is_just_pressed(BUTTON_A | BUTTON_START)) {
             _dialogue_engine_prepare_page(DIALOGUE_BOX_CONTENT_CHAR_WIDTH, DIALOGUE_BOX_CONTENT_MAX_LINES); 
             // If after preparing, num_lines is 0 and no more pages, it means we just finished.
             if (current_dialogue.num_lines_on_current_page == 0 && !current_dialogue.needs_paging_indicator) {
                 current_dialogue.is_active = FALSE;
             }
             return TRUE; // Indicate an update happened (page turn or dialogue ended)
        }
    } else { 
        // No more pages, but dialogue is still "active" until user dismisses the last page
        if (input_is_just_pressed(BUTTON_A | BUTTON_START)) {
            current_dialogue.is_active = FALSE; // Mark as inactive
            return TRUE; // Indicate an update happened (dialogue ended by user action)
        }
    }
    return FALSE; // No change in dialogue state this frame due to input
}

u8 dialogue_engine_is_active() {
    return current_dialogue.is_active;
}
