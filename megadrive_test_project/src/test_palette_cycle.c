#include "test_palette_cycle.h"
#include "input.h"
#include <genesis.h> // For VDP functions, u16, etc.
#include <string.h>  // For memcpy

// Palette indices in PAL0 to be cycled
#define CYCLE_IDX_1 1 // Palette color index 1
#define CYCLE_IDX_2 2 // Palette color index 2
#define CYCLE_IDX_3 3 // Palette color index 3

// Number of colors in our cycle
#define NUM_CYCLE_COLORS 3
// The actual color values for the cycle (e.g., Red, Green, Blue)
static const u16 cycle_colors[NUM_CYCLE_COLORS] = {
    RGB24_TO_VDPCOLOR(0xFF0000), // Red
    RGB24_TO_VDPCOLOR(0x00FF00), // Green
    RGB24_TO_VDPCOLOR(0x0000FF)  // Blue
};

static u16 base_palette0[16];          // To store the initial state of PAL0
static s16 current_cycle_step = 0;     // Which color in cycle_colors is active for CYCLE_IDX_1
static u16 cycle_timer = 0;
#define CYCLE_SPEED 15 // Update every 15 frames (0.25s at 60Hz)

void palette_cycle_test_init() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    // Store current PAL0, then set up our base for this test
    VDP_getPalette(PAL0, base_palette0);

    u16 temp_pal0[16];
    memcpy(temp_pal0, base_palette0, sizeof(base_palette0)); // Start with a copy of original PAL0

    // Set initial colors for the cycle indices
    temp_pal0[CYCLE_IDX_1] = cycle_colors[0];
    temp_pal0[CYCLE_IDX_2] = cycle_colors[1];
    temp_pal0[CYCLE_IDX_3] = cycle_colors[2];
    temp_pal0[15] = RGB24_TO_VDPCOLOR(0xFFFFFF); // Ensure color 15 is white for default text

    VDP_setPalette(PAL0, temp_pal0);

    VDP_setTextPalette(PAL0); // Use PAL0 for text

    // Create 3 tiles, each a solid color using palette indices 1, 2, and 3 respectively.
    // VDP_fillTileData(fill_value, tile_index, count, wait_dma_queue)
    // For 4bpp tiles (default), each byte defines two pixels.
    // 0x11 means both pixels use color index 1 from the palette.
    // 0x22 means both pixels use color index 2, etc.
    VDP_fillTileData(0x11, TILE_USER_INDEX + 0, 1, FALSE); // Tile for PAL0[CYCLE_IDX_1]
    VDP_fillTileData(0x22, TILE_USER_INDEX + 1, 1, FALSE); // Tile for PAL0[CYCLE_IDX_2]
    VDP_fillTileData(0x33, TILE_USER_INDEX + 2, 1, FALSE); // Tile for PAL0[CYCLE_IDX_3]
    VDP_waitDMACompletion(); // Ensure tile data is written before drawing

    // Draw rows of these solid color tiles
    // TILE_ATTR_FULL(palette_num, priority, v_flip, h_flip, tile_vram_index)
    // We use PAL0 for these tiles.
    for(int i=0; i<10; ++i) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX + 0), 5+i, 8);
    for(int i=0; i<10; ++i) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX + 1), 5+i, 10);
    for(int i=0; i<10; ++i) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX + 2), 5+i, 12);
    
    VDP_drawText("Cycling PAL0 indices 1,2,3", 2, 2);
    VDP_drawText("Row 1 uses PAL0[1]", 18, 8);
    VDP_drawText("Row 2 uses PAL0[2]", 18, 10);
    VDP_drawText("Row 3 uses PAL0[3]", 18, 12);
    VDP_drawText("Press Start to Exit", 2, 26);

    current_cycle_step = 0;
    cycle_timer = 0;
}

void palette_cycle_test_update() {
    cycle_timer++;
    if (cycle_timer >= CYCLE_SPEED) {
        cycle_timer = 0;
        current_cycle_step = (current_cycle_step + 1) % NUM_CYCLE_COLORS;

        // Cycle the colors:
        // Color at PAL0[CYCLE_IDX_1] gets current_step color
        // Color at PAL0[CYCLE_IDX_2] gets next color in cycle_colors
        // Color at PAL0[CYCLE_IDX_3] gets next-next color in cycle_colors
        VDP_setPaletteColor(CYCLE_IDX_1, cycle_colors[(current_cycle_step + 0) % NUM_CYCLE_COLORS]);
        VDP_setPaletteColor(CYCLE_IDX_2, cycle_colors[(current_cycle_step + 1) % NUM_CYCLE_COLORS]);
        VDP_setPaletteColor(CYCLE_IDX_3, cycle_colors[(current_cycle_step + 2) % NUM_CYCLE_COLORS]);
    }
    // Exit condition (Start button press) is handled in main.c's game loop
}

void palette_cycle_test_on_exit() {
    // Restore original PAL0
    VDP_setPalette(PAL0, base_palette0);
    // Other cleanup (like clearing specific tiles or text) can be done here if needed,
    // but main.c's return_to_menu() already clears the planes.
}
