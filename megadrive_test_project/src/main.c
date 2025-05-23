/**
 * @file main.c
 * @brief Main application entry point for the Sega Mega Drive Test Project.
 *
 * This file contains the `main()` function, which serves as the starting point
 * of the application. It initializes various systems including SGDK, input,
 * sound, and graphics. It implements a state machine to manage different parts
 * of the application such as a loading screen, a main menu, and various test modules.
 * The main game loop calls update functions based on the current state.
 */
#include <genesis.h>      // SGDK's main header file. Provides access to Mega Drive hardware functions.
#include "graphics.h"     // For sprite and tile graphics functions.
#include "resources.h"    // For compiled game assets (images, sounds etc. via rescomp).
#include "transitions.h"  // For screen fade effects.
#include "input.h"        // For controller input handling.
#include "sound.h"        // For sound effect playback.
#include "menu.h"         // Include the menu system header
#include "input_test.h"   // Include the input test display header

//--------------------------------------------------------------------------------------------------
// Game State Definition and Management
//--------------------------------------------------------------------------------------------------

/**
 * @brief Defines the different states the application can be in.
 * Each state corresponds to a major section or mode of the application.
 */
typedef enum {
    STATE_LOADING_SCREEN,       ///< Displays the initial loading/splash screen.
    STATE_MENU,                 ///< Displays the main menu system.
    STATE_TEST_SPRITE_DEMO,     ///< Runs the interactive sprite demonstration.
    STATE_TEST_TILEMAP_DISPLAY, ///< Runs the tilemap display test.
    STATE_TEST_FADES,           ///< Runs the screen fade effects test.
    STATE_TEST_INPUT_DISPLAY    ///< Runs the controller input display test.
} GameState;

/**
 * @brief Global variable holding the current game state.
 * The main loop uses this variable to determine which update function to call.
 */
static GameState current_game_state;

// --- Variables and Enum for Fade Test (specific to STATE_TEST_FADES) ---
static u16 fade_test_palette[16]; // To store a custom palette for the fade test
typedef enum {
    FTS_INIT, FTS_SHOW_INITIAL, FTS_FADING_OUT, FTS_WAIT_BLACK, FTS_FADING_IN, FTS_DONE
} FadeTestSubState;
static FadeTestSubState fade_test_current_sub_state;
static u32 fade_test_timer;


//--------------------------------------------------------------------------------------------------
// Forward Declarations for State Functions
//--------------------------------------------------------------------------------------------------

// --- Initialization and Transition Functions ---
static void show_loading_screen();
static void go_to_menu_state();
static void return_to_menu();
static void init_sprite_demo_state();
static void init_tilemap_display_state();
static void init_fades_test_state();
static void init_input_display_state();

// --- Update Functions ---
static void update_menu_state();
static void update_test_sprite_demo();
static void update_fades_test_state();
static void update_input_display_state();


//--------------------------------------------------------------------------------------------------
// State Initialization and Transition Functions
//--------------------------------------------------------------------------------------------------

/**
 * @brief Displays the initial loading screen.
 *
 * This function is the first active part of the application after basic SGDK setup.
 * It clears the screen, displays the "Minnka" logo, and waits for a set duration
 * or a Start button press. It then transitions to the main menu.
 */
static void show_loading_screen() {
    u32 timer_start_time;
    const u16 loading_screen_duration_seconds = 3;

    SPR_end(); // Ensure sprites are off during loading screen
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    // Load and display the logo (compiled via resources.res)
    VDP_setPalette(PAL0, logo_minnka_img_tileset.palette->data);
    VDP_loadTileSet(&logo_minnka_img_tileset, TILE_USER_INDEX, DMA);

    u16 logo_offset_x = 0;
    if (logo_minnka_img_tilemap.w < 40) { // Center if narrower than 40 tiles (320px)
        logo_offset_x = (40 - logo_minnka_img_tilemap.w) / 2;
    }

    VDP_setTileMapEx(BG_A, &logo_minnka_img_tilemap,
                     TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX),
                     logo_offset_x, 0, 0, 0,
                     logo_minnka_img_tilemap.w, logo_minnka_img_tilemap.h, DMA);

    timer_start_time = SYS_getTime(); // Get time in system ticks

    // Loop for duration or until Start is pressed
    while(TRUE) {
        input_update(); // Must be called to read joypad state
        if (input_is_button_pressed(BUTTON_START)) break;
        if (SYS_getTime() - timer_start_time >= (loading_screen_duration_seconds * SGDK_TIMER_NORMAL_DIV)) break;
        SYS_doVBlankProcess(); // Process VBlank tasks (like VSync wait)
    }
    go_to_menu_state(); // Transition to the main menu
}

/**
 * @brief Transitions the game to the main menu state.
 *
 * This function is called after the loading screen or when exiting a test state.
 * It prepares the screen for the menu, calls `menu_init()` to set up menu graphics
 * and logic, and sets the `current_game_state` to `STATE_MENU`.
 */
static void go_to_menu_state() {
    VDP_clearPlane(BG_A, TRUE); // Clear any previous content from planes
    VDP_clearPlane(BG_B, TRUE);
    // menu_init() will set its own background color and text palette.
    menu_init(); // Initialize menu content, drawing, and internal state
    current_game_state = STATE_MENU; // Set the application state to Menu
}

/**
 * @brief Handles returning to the main menu from a test state.
 *
 * This function performs common cleanup when exiting a test:
 * - Disables sprites using `SPR_end()`.
 * - Clears VDP planes `BG_A` and `BG_B`.
 * It then calls `go_to_menu_state()` to re-initialize and display the menu.
 * Optional fade transitions could be added here for smoother exits from tests.
 */
static void return_to_menu() {
    SPR_end(); // Clear/disable all sprites
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    // Optional: transition_fade_out_to_black(10); // Fade out from test
    go_to_menu_state();
    // Optional: transition_fade_in_from_black(10);  // Fade into menu
}

/**
 * @brief Initializes the sprite demonstration test state.
 *
 * Prepares the system for the interactive sprite demo. This includes:
 * - Clearing VDP planes and setting a background color for the demo.
 * - Calling `setup_sprites()` to initialize sprite data, palettes, and related systems.
 * - Storing current palettes and performing a fade-out/fade-in transition.
 * - Setting the `current_game_state` to `STATE_TEST_SPRITE_DEMO`.
 */
static void init_sprite_demo_state() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_setPaletteColor(0, RGB24_TO_VDPCOLOR(0x000040)); // Background color for sprite demo
    setup_sprites(); // Initializes sprites, loads palettes, etc.
    store_current_palettes(); // Store palettes for the fade effect
    transition_fade_out_to_black(15); // Quick fade out before starting
    transition_fade_in_from_black(15);  // Quick fade in to the demo
    current_game_state = STATE_TEST_SPRITE_DEMO;
}

/**
 * @brief Initializes the tilemap display test state.
 *
 * Prepares and displays a static tilemap. This involves:
 * - Clearing VDP planes.
 * - Calling `load_simple_tileset()` to load tile graphics and palette.
 * - Calling `display_simple_tilemap()` to draw the map on `BG_A`.
 * - Displaying an exit message.
 * - Setting the `current_game_state` to `STATE_TEST_TILEMAP_DISPLAY`.
 */
static void init_tilemap_display_state() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    load_simple_tileset();    // Loads tileset (my_tileset) and its palette (to PAL0)
    display_simple_tilemap(); // Draws the predefined map from graphics.c on BG_A
    VDP_drawText("Tilemap Test - Press Start to Exit", 2, 26); // Position text at bottom
    current_game_state = STATE_TEST_TILEMAP_DISPLAY;
}

/**
 * @brief Initializes the screen fades test state.
 *
 * Sets up a demonstration of fade-out and fade-in transitions. This involves:
 * - Clearing VDP planes.
 * - Creating and setting a custom bright palette for `PAL0`.
 * - Drawing sample text using this palette.
 * - Storing current palettes for the transition functions.
 * - Initializing the fade test's internal sub-state machine.
 * - Setting the `current_game_state` to `STATE_TEST_FADES`.
 */
static void init_fades_test_state() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);

    // Create a simple, bright palette for PAL0 for this test
    fade_test_palette[0] = RGB24_TO_VDPCOLOR(0x222222); // Dark grey background
    fade_test_palette[1] = RGB24_TO_VDPCOLOR(0xFF0000); // Red
    fade_test_palette[2] = RGB24_TO_VDPCOLOR(0x00FF00); // Green
    fade_test_palette[3] = RGB24_TO_VDPCOLOR(0x0000FF); // Blue
    for(int i=4; i<16; ++i) fade_test_palette[i] = fade_test_palette[0]; // Fill rest
    VDP_setPalette(PAL0, fade_test_palette);

    // Draw some colored text/elements to showcase the fade
    VDP_setTextPalette(PAL0);
    VDP_drawText("Red", 12, 12);
    VDP_drawText("Green", 12, 14);
    VDP_drawText("Blue", 12, 16);
    VDP_drawText("Watch the Fades!", 10, 5);

    store_current_palettes(); // Store PAL0 (custom bright one) and PAL1 (likely default/black)

    fade_test_current_sub_state = FTS_INIT; // Initialize the sub-state for the fade test
    current_game_state = STATE_TEST_FADES;
}

/**
 * @brief Initializes the controller input display test state.
 *
 * Prepares for the input test by calling `input_test_init_display()` (from `input_test.c`),
 * which sets up the screen for displaying controller input.
 * Sets the `current_game_state` to `STATE_TEST_INPUT_DISPLAY`.
 */
static void init_input_display_state() {
    input_test_init_display(); // Sets up the screen for the input test
    current_game_state = STATE_TEST_INPUT_DISPLAY;
}


//--------------------------------------------------------------------------------------------------
// State Update Functions
//--------------------------------------------------------------------------------------------------

/**
 * @brief Updates the main menu state.
 *
 * This function is called when `current_game_state` is `STATE_MENU`.
 * It processes menu input using `menu_handle_input()` and checks if a menu
 * item has been selected via `menu_get_selected_action_id()`. If an item is
 * selected, it resets the action ID and transitions to the corresponding
 * test state by calling its initialization function (e.g., `init_sprite_demo_state()`).
 * `menu_draw()` is handled internally by `menu_init()` and `menu_handle_input()`.
 */
static void update_menu_state() {
    // input_update() is called at the start of the main game loop.
    menu_handle_input(); // Processes D-Pad navigation and Start/A button selection

    s16 selected_test_index = menu_get_selected_action_id();

    if (selected_test_index != -1) { // An item has been selected
        menu_reset_action_id(); // Reset the action ID to prevent re-triggering

        switch (selected_test_index) {
            case 0: init_sprite_demo_state(); break;
            case 1: init_tilemap_display_state(); break;
            case 2: init_fades_test_state(); break;
            case 3: init_input_display_state(); break;
            default: go_to_menu_state(); break; // Should not happen
        }
    }
}

/**
 * @brief Updates logic for the sprite demonstration test state.
 *
 * Calls `update_sprites_example()` (from `graphics.c`) for the interactive sprite demo.
 * Checks for the Start button press to return to the main menu.
 */
static void update_test_sprite_demo() {
    update_sprites_example(); // Contains sprite movement, animation, sound logic
    if (input_is_button_pressed(BUTTON_START)) {
        return_to_menu();
    }
}

/**
 * @brief Updates logic for the screen fades test state.
 *
 * Manages the sequence of the fade test through its sub-states (init, showing initial,
 * fading out, waiting in black, fading in, done). Uses `fade_test_timer` and
 * `SYS_getTime()` to control durations. Calls `transition_fade_out_to_black()` and
 * `transition_fade_in_from_black()`.
 * Checks for the Start button press to return to the main menu.
 */
static void update_fades_test_state() {
    switch (fade_test_current_sub_state) {
        case FTS_INIT:
            VDP_drawText("Fade Test. Will start in 2s.", 5, 20);
            fade_test_timer = SYS_getTime();
            fade_test_current_sub_state = FTS_SHOW_INITIAL;
            break;
        case FTS_SHOW_INITIAL:
            if (SYS_getTime() - fade_test_timer > (2 * SGDK_TIMER_NORMAL_DIV)) {
                VDP_clearText(5, 20, 30);
                VDP_drawText("Fading Out...", 10, 20);
                transition_fade_out_to_black(60); // 1-second fade out
                fade_test_timer = SYS_getTime();
                fade_test_current_sub_state = FTS_WAIT_BLACK;
            }
            break;
        case FTS_WAIT_BLACK:
            if (SYS_getTime() - fade_test_timer > (1 * SGDK_TIMER_NORMAL_DIV)) {
                VDP_clearText(10, 20, 30);
                VDP_drawText("Fading In...", 10, 20);
                transition_fade_in_from_black(60); // 1-second fade in
                fade_test_current_sub_state = FTS_DONE;
            }
            break;
        case FTS_DONE:
            VDP_clearText(10, 20, 30);
            VDP_drawText("Fade Test Complete. Press Start.", 2, 26);
            // No automatic transition from here, user exits with Start.
            break;
    }
    if (input_is_button_pressed(BUTTON_START)) {
        return_to_menu();
    }
}

/**
 * @brief Updates logic for the controller input display test state.
 *
 * Calls `input_test_update_display()` (from `input_test.c`) with the current
 * joypad state obtained from `input_get_joy1_state()`.
 * Checks for the Start button press to return to the main menu.
 */
static void update_input_display_state() {
    input_test_update_display(input_get_joy1_state());
    if (input_is_button_pressed(BUTTON_START)) {
        return_to_menu();
    }
}


//--------------------------------------------------------------------------------------------------
// Main Application Entry Point
//--------------------------------------------------------------------------------------------------

/**
 * @brief Main function - the entry point of the application.
 *
 * Initializes SGDK and all custom game modules (input, sound).
 * Sets the initial game state to `STATE_LOADING_SCREEN`.
 * Enters the main game loop, which:
 * 1. Updates controller input using `input_update()`.
 * 2. Uses a `switch` statement based on `current_game_state` to call the
 *    appropriate update function for the current state.
 * 3. Calls `SYS_doVBlankProcess()` to handle VBlank tasks (sprite DMA, sound updates, VSync wait).
 *
 * @return int Typically 0, though the return value is not used in this embedded context.
 */
int main() {
    // Initialize SGDK. This also initializes VDP, sound (Z80), and JOY manager.
    SGDK_init();

    // Initialize custom project-wide modules
    input_init();        // Input handling system
    init_sound_system(); // Sound system (basic PCM playback)

    // Set the initial game state
    current_game_state = STATE_LOADING_SCREEN;

    // --- Main Game Loop ---
    while(1) {
        // Update controller input state once per frame
        input_update();

        // Process logic based on the current game state
        switch (current_game_state) {
            case STATE_LOADING_SCREEN:
                show_loading_screen(); // This function handles its own loop and state transition
                break;
            case STATE_MENU:
                update_menu_state();   // Handles menu navigation and test selection
                break;
            case STATE_TEST_SPRITE_DEMO:
                update_test_sprite_demo();
                break;
            case STATE_TEST_TILEMAP_DISPLAY:
                // For static display tests, only input check for exit is needed here.
                // The display itself is set up in init_tilemap_display_state().
                if (input_is_button_pressed(BUTTON_START)) { return_to_menu(); }
                break;
            case STATE_TEST_FADES:
                update_fades_test_state(); // Manages its own sub-state and display updates
                // Exit via Start button is handled within update_fades_test_state()
                break;
            case STATE_TEST_INPUT_DISPLAY:
                update_input_display_state(); // Updates the displayed input values
                // Exit via Start button is handled within update_input_display_state()
                break;
            default:
                // Should not happen with defined states.
                // Optionally, handle by defaulting to a safe state like the menu.
                // go_to_menu_state();
                break;
        }
        // SGDK's VBlank processing function.
        // Handles VBlank tasks: DMA sprite updates, sound driver updates, VSync wait.
        SYS_doVBlankProcess();
    }
    return (0); // Standard main return, not typically used in embedded systems.
}
