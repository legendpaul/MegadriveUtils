#include "test_scrolling.h"
#include "scrolling_map_data.h" // Our new map data
#include "input.h"
#include "resources.h" // For my_tileset
#include <string.h>    // For KLog or sprintf if used for debug text

static s16 scroll_x_px = 0;
static s16 scroll_y_px = 0;
#define SCROLL_SPEED 2 // pixels per frame

// Max scroll values depend on map size and screen size
// Screen: 320x224 pixels (40x28 tiles)
// Map: SCROLLING_MAP_WIDTH * 8 x SCROLLING_MAP_HEIGHT * 8 pixels
// Max scroll X = (Map Width in pixels) - (Screen Width in pixels)
// Max scroll Y = (Map Height in pixels) - (Screen Height in pixels)
#define MAX_SCROLL_X ((SCROLLING_MAP_WIDTH * 8) - 320)
#define MAX_SCROLL_Y ((SCROLLING_MAP_HEIGHT * 8) - 224)


void scrolling_test_init() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE); // Clear other plane too

    // Set plane size for BG_A to accommodate the large map
    // SGDK plane sizes: 32, 64, 128 tiles.
    // Our map is 64x32 tiles. This fits a 64x32 plane.
    // For SGDK, actual plane width/height arguments are in tiles: (width_tiles, height_tiles)
    // The VDP_setPlaneSize function takes these tile counts.
    // SGDK will select the smallest VDP plane size that can contain these dimensions.
    // e.g., if map is 64x32 tiles, it will use a 64x32 VRAM plane.
    // If map is 40x30, it might use a 64x32 VRAM plane.
    VDP_setPlaneSize(BG_A, SCROLLING_MAP_WIDTH, SCROLLING_MAP_HEIGHT, FALSE);

    // Load the tileset (my_tileset from resources.res, using tileset.png)
    // This assumes my_tileset uses PAL0, or we need to load its palette.
    // load_simple_tileset() from graphics.c loads my_tileset and its palette to PAL0.
    // Let's ensure it's called or that the palette is correctly set.
    // For now, assuming my_tileset.palette is available and loaded to PAL0 by a previous step
    // or that this test should explicitly load it.
    // The `load_simple_tileset()` from graphics.c does:
    //   VDP_loadTileSet(&my_tileset, TILE_USER_INDEX, DMA);
    //   VDP_setPalette(PAL0, my_tileset.palette->data);
    // We should do the same here, or rely on it being done if this test is part of a larger system.
    // For a self-contained test, let's load it.
    VDP_loadTileSet(&my_tileset, TILE_USER_INDEX, DMA);
    VDP_setPalette(PAL0, my_tileset.palette->data);


    // Draw the large map onto BG_A
    // VDP_setTileMapData efficiently copies tile data for the entire map.
    // Arguments: plane, source_tilemap_data, dest_x_tile_plane, dest_y_tile_plane,
    //            width_tiles, height_tiles, source_map_is_same_width_as_plane, transfer_method
    VDP_setTileMapData(BG_A, (const u16*)scrolling_map_data, 0, 0, SCROLLING_MAP_WIDTH, SCROLLING_MAP_HEIGHT, TRUE, DMA);
    // The TRUE above is for plan_width_is_tilemap_width which is true here since we sized the plane to the map.

    // Set initial scroll position
    scroll_x_px = 0;
    scroll_y_px = 0;
    VDP_setHorizontalScroll(BG_A, scroll_x_px);
    VDP_setVerticalScroll(BG_A, scroll_y_px);

    VDP_setTextPalette(PAL0); // Text will use PAL0 (same as map for now)
    VDP_drawText("Scrolling Demo. Use D-Pad.", 2, 2);
    VDP_drawText("Press Start to Exit.", 2, 3);
}

void scrolling_test_update() {
    if (input_is_dpad_left_pressed()) {
        scroll_x_px -= SCROLL_SPEED;
        if (scroll_x_px < 0) scroll_x_px = 0;
    }
    if (input_is_dpad_right_pressed()) {
        scroll_x_px += SCROLL_SPEED;
        if (scroll_x_px > MAX_SCROLL_X) scroll_x_px = MAX_SCROLL_X;
    }
    if (input_is_dpad_up_pressed()) {
        scroll_y_px -= SCROLL_SPEED;
        if (scroll_y_px < 0) scroll_y_px = 0;
    }
    if (input_is_dpad_down_pressed()) {
        scroll_y_px += SCROLL_SPEED;
        if (scroll_y_px > MAX_SCROLL_Y) scroll_y_px = MAX_SCROLL_Y;
    }

    VDP_setHorizontalScroll(BG_A, scroll_x_px);
    VDP_setVerticalScroll(BG_A, scroll_y_px);

    // Display scroll coordinates (optional, for debugging)
    char coord_text[30];
    sprintf(coord_text, "X:%4d Y:%3d", scroll_x_px, scroll_y_px); // Padded for consistent clear
    VDP_clearText(2, 5, 20); // Clear previous text based on max length of coord_text
    VDP_drawText(coord_text, 2, 5);
}

void scrolling_test_on_exit() {
    // Reset plane size to a common default (e.g., 64x32 or 32x32) if it was changed.
    // The menu system and other tests might expect a certain plane size.
    // SGDK's default is often 64x32 for BG_A/BG_B after SGDK_init.
    // If SCROLLING_MAP_WIDTH/HEIGHT are different from this, reset.
    // For now, we used SCROLLING_MAP_WIDTH (64) and SCROLLING_MAP_HEIGHT (32),
    // so it might already be the default or a common size.
    // If a different test needs a smaller plane, it should set it in its init.
    // Best practice is to explicitly reset if unsure or if it differs from a known common state.
    // VDP_setPlaneSize(BG_A, 64, 32, FALSE); // Example reset to 64x32

    // Clear scroll registers
    VDP_setHorizontalScroll(BG_A, 0);
    VDP_setVerticalScroll(BG_A, 0);

    // Clear the plane so the map doesn't persist into the menu
    VDP_clearPlane(BG_A, TRUE);
    // Palette will be reset by menu_init's VDP_setPaletteColor(0, ...) for background
    // and VDP_setTextPalette().
}
