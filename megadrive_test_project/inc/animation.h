/**
 * @file animation.h
 * @brief Header file for sprite animation functions.
 *
 * This module handles frame-by-frame animation updates for sprites.
 * It currently provides a simple mechanism to cycle through animation frames
 * for a given sprite based on a timer.
 */
#ifndef ANIMATION_H
#define ANIMATION_H

#include <genesis.h> // SGDK general header

// Structure to hold animation state if needed for more complex scenarios
// For this simple case, we might not need it if graphics.c manages the Sprite* directly.
// typedef struct {
//     Sprite* sprite_object; // The SGDK sprite object
//     u16 current_frame;
//     u16 num_frames;
//     u16 anim_timer;
//     u16 anim_speed; // ticks per frame
// } AnimatedSprite;

// void init_animation(AnimatedSprite* anim_sprite, Sprite* sgdk_sprite, u16 num_frames, u16 anim_speed);

/**
 * @brief Updates the animation frame for the player's sprite.
 *
 * This function should be called once per game loop. It increments an internal timer
 * and changes the sprite's displayed frame when the timer reaches a threshold,
 * effectively controlling the animation speed.
 *
 * @param player_sprite_sgdk Pointer to the SGDK Sprite object for the player.
 *                           This sprite's frame will be updated by SPR_setFrame().
 */
void update_player_animation(Sprite* player_sprite_sgdk);

#endif // ANIMATION_H
