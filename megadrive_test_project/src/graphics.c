/**
 * @file graphics.c
 * @brief Implements graphics rendering functions.
 *
 * This module handles the setup and updating of visual elements such as
 * tilemaps and sprites. It includes functions for loading graphical assets,
 * initializing the sprite system, and managing player sprite movement and animation.
 */
#include "graphics.h"
#include "resources.h" // For rescomp resources (spr_player, my_tileset, sfx_ping_data)
#include "animation.h" // For update_player_animation()
#include "pcm_player.h"  // New - For pcm_player_play()
#include "input.h"     // For input_is_dpad_xxx_pressed() and input_is_button_pressed()

// --- Tilemap Definition (Example) ---
// This is a sample tilemap that can be displayed.
// It's currently not active in the main demo loop.
// Tile indices correspond to tiles in 'tileset.png' after processing by rescomp.
// 0: Transparent/empty (or first tile in tileset if not handled as special)
// 1: White tile (index 1 in tileset.png)
// 2: Pattern A tile (index 2 in tileset.png)
// 3: Pattern B tile (index 3 in tileset.png)

/** @brief Width of the example tilemap in tiles. */
#define MAP_WIDTH 20
/** @brief Height of the example tilemap in tiles. */
#define MAP_HEIGHT 10

/**
 * @brief Defines the layout of the simple example tilemap.
 * Each value is a tile index that refers to a tile in `my_tileset`.
 */
const u16 simple_map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,2,2,0,0,0,0,0,0,0,0,0,2,2,0,0,0,1},
    {1,0,0,2,2,0,0,0,0,0,0,0,0,0,2,2,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,3,0,0,3,0,0,0,0,0,0,0,0,1},
    {1,2,3,0,0,0,0,3,3,3,3,0,0,0,0,0,0,2,3,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// --- Sprite Variables ---
/** @brief Pointer to the SGDK Sprite object for the player. */
static Sprite* player_sprite_sgdk;
/** @brief Player's current X position on the screen. */
static s16 player_x = 100;
/** @brief Player's current Y position on the screen. */
static s16 player_y = 100;
/** @brief Movement speed of the player sprite in pixels per frame. */
#define PLAYER_SPEED 2


// --- Function Implementations ---

/**
 * @brief Loads the simple tileset and its palette into VRAM.
 *
 * The tileset data (`my_tileset`) is defined in `resources.h` (generated from `resources.res`)
 * and originates from `res/gfx/tileset.png`.
 * Tiles are loaded into VRAM starting at `TILE_USER_INDEX`.
 * The palette associated with `my_tileset` is loaded into hardware palette `PAL0`.
 */
void load_simple_tileset() {
    // VDP_loadTileSet arguments:
    // const TileSet *tileset: Pointer to the TileSet structure (from resources.h).
    // u16 index: The starting tile index in VRAM where tiles will be loaded.
    //            `TILE_USER_INDEX` is an SGDK constant for the first user-definable tile index.
    // TransferMethod tm: DMA is recommended for speed.
    VDP_loadTileSet(&my_tileset, TILE_USER_INDEX, DMA);

    // Load palette for the tileset.
    // `my_tileset.palette` is a pointer to a Palette structure (also from resources.h).
    // `PAL0` is an SGDK constant for hardware palette 0.
    VDP_setPalette(PAL0, my_tileset.palette->data);
}

/**
 * @brief Displays the `simple_map` on background plane A (BG_A).
 *
 * This function first clears BG_A. Then, it iterates through the `simple_map`
 * array and uses `VDP_setTileMapXY` to draw each tile onto the screen.
 * Tiles with value 0 in `simple_map` are considered empty and are skipped.
 * The tile attributes are set to use `PAL0` and the tile index is offset by `TILE_USER_INDEX`.
 */
void display_simple_tilemap() {
    VDP_clearPlane(BG_A, TRUE); // Clear Plane A, TRUE to also clear VRAM representation

    for (int y_coord = 0; y_coord < MAP_HEIGHT; y_coord++) {
        for (int x_coord = 0; x_coord < MAP_WIDTH; x_coord++) {
            // Only draw non-zero tiles (assuming 0 is an "empty" tile in our map design)
            if (simple_map[y_coord][x_coord] != 0) {
                // Construct tile attributes:
                // - PAL0: Use palette 0.
                // - Priority: FALSE (lower priority than sprites if sprites have high priority).
                // - FlipH, FlipV: FALSE (no flipping).
                // - Tile Index: `TILE_USER_INDEX` + value from `simple_map`.
                //   This assumes tile values in `simple_map` (1, 2, 3, etc.) directly
                //   correspond to the order of tiles in `tileset.png` (after the 0th tile).
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX + simple_map[y_coord][x_coord]),
                                 x_coord, y_coord);
            }
        }
    }
}

/**
 * @brief Initializes the sprite system and the player sprite.
 *
 * This function performs critical setup for using sprites:
 * 1.  Calls `SPR_init()` to initialize SGDK's sprite engine.
 * 2.  Loads the player sprite's palette (`spr_player.palette` from `resources.h`)
 *     into hardware palette `PAL1` using `VDP_setPalette()`.
 * 3.  Adds the player sprite to the VDP sprite list using `SPR_addSprite()`.
 *     `&spr_player` (from `resources.h`) provides the sprite definition (size, tiles, etc.).
 *     Initial position is `(player_x, player_y)`.
 *     Attributes `TILE_ATTR(PAL1, TRUE, FALSE, FALSE)` set palette to `PAL1` and high priority.
 * 4.  Sets the initial animation frame of the player sprite to frame 0 using `SPR_setFrame()`.
 *
 * Note: `init_sound_system()` and `input_init()` are also called here, though they could
 * alternatively be called directly from `main.c` during global initialization.
 */
void setup_sprites() {
    SPR_init(); // Initialize SGDK sprite engine

    // Load player sprite palette into PAL1
    VDP_setPalette(PAL1, spr_player.palette->data);

    // Add player sprite to the screen
    player_sprite_sgdk = SPR_addSprite(&spr_player, player_x, player_y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));

    // Set initial animation frame
    SPR_setFrame(player_sprite_sgdk, 0);

    // Initialize other systems (can also be done in main)
    // init_sound_system(); // REMOVED - Sound system init called from main.c now by sound_manager_init()
    // input_init();      // Input system init called from main.c now
}

/**
 * @brief Updates game logic related to sprites, primarily player movement and actions.
 *
 * This function is called every frame from the main game loop. It performs:
 * 1.  **Input Handling:** Checks D-Pad input using `input_is_dpad_xxx_pressed()`
 *     functions and updates `player_x` and `player_y` positions based on `PLAYER_SPEED`.
 * 2.  **Boundary Checks:** Ensures the player sprite stays within the screen boundaries
 *     (320x224, considering sprite size 16x16).
 * 3.  **Sprite Position Update:** Sets the new hardware sprite position using `SPR_setPosition()`.
 * 4.  **Sound Trigger:** Checks if Button A is pressed using `input_is_button_pressed(BUTTON_A)`.
 *     If true, it calls `play_sfx_ping()` to play the sound effect.
 * 5.  **Animation Update:** Calls `update_player_animation()` to advance the player's
 *     animation frame if necessary.
 * 6.  **VDP Update:** Calls `SPR_update()` to commit all sprite changes (position, frame, etc.)
 *     to the VDP for display on the next screen refresh.
 */
void update_sprites_example() {
    // --- Input-based movement ---
    if (input_is_held(BUTTON_LEFT)) { // Was input_is_dpad_left_pressed or similar
        player_x -= PLAYER_SPEED;
    }
    if (input_is_held(BUTTON_RIGHT)) {
        player_x += PLAYER_SPEED;
    }
    if (input_is_held(BUTTON_UP)) {
        player_y -= PLAYER_SPEED;
    }
    if (input_is_held(BUTTON_DOWN)) {
        player_y += PLAYER_SPEED;
    }

    // Keep sprite on screen (simple boundary check)
    // Screen width 320, height 224. Sprite is 16x16 pixels.
    if (player_x < 0) player_x = 0;
    if (player_x > (320 - 16)) player_x = 320 - 16;
    if (player_y < 0) player_y = 0;
    if (player_y > (224 - 16)) player_y = 224 - 16;

    // Update hardware sprite position
    SPR_setPosition(player_sprite_sgdk, player_x, player_y);

    // --- Sound Trigger ---
    // Play sound if Button A is pressed
    if (input_is_button_pressed(BUTTON_A)) {
        pcm_player_play(&sfx_ping_data); // Replaced play_sfx_ping()
    }

    // Update player animation
    update_player_animation(player_sprite_sgdk);

    // Commit all sprite changes to VDP
    SPR_update();
}
