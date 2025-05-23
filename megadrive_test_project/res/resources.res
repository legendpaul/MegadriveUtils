# SGDK Resource Definition File
# This file tells the 'rescomp' tool (part of SGDK) how to process and
# include game assets into the project. Each line defines a resource.

# --- Graphics Resources ---

# IMAGE: Defines an image resource.
# 'tileset_img': The C variable name that will be generated for this image in resources.h.
#                This is useful if you want to access the raw Image structure.
# "gfx/tileset.png": Path to the source image file, relative to this .res file or project root.
# BEST: Compression method. 'BEST' tries to find the best compression (usually LZ4W).
#       Other options include NONE, LZ4W, APLIB.
# ALL_PALETTE: Specifies that the entire palette from the image should be included.
#              Alternatively, you can specify a fixed number of colors or a specific palette index.
IMAGE tileset_img "gfx/tileset.png" BEST ALL_PALETTE

# TILESET: Defines a tileset resource from an image.
# 'my_tileset': The C variable name for the TileSet structure in resources.h.
#               This structure contains tile data, palette, and dimensions.
# "gfx/tileset.png": Path to the source image. Tiles are typically 8x8 pixels.
# BEST: Compression method for tile data.
# ALL_PALETTE: Palette handling, similar to IMAGE.
TILESET my_tileset "gfx/tileset.png" BEST ALL_PALETTE

# SPRITE: Defines a sprite resource from an image.
# 'spr_player': The C variable name for the SpriteDefinition structure in resources.h.
# "gfx/sprite_player.png": Path to the sprite sheet image.
# 2 2: Sprite size in tiles (width x height). '2 2' means 2 tiles wide and 2 tiles high (16x16 pixels).
#      If the source image is wider than (width_tiles * 8) pixels,
#      rescomp interprets the extra width as animation frames.
#      e.g., sprite_player.png is 32x16. With 2x2 tiles, it's 16px wide for one frame.
#      32px width / 16px per frame = 2 animation frames.
# NONE: Compression for sprite data. Sprites often use NONE for faster VRAM transfer,
#       but compression (e.g., LZ4W) is possible.
# 0: Time per frame in game ticks (1/60th or 1/50th of a second).
#    A value of 0 means the animation speed is not set by rescomp and should be
#    handled manually in code (e.g., using SPR_setAnimAndFrame() or custom timers).
#    This is generally preferred for more control.
SPRITE spr_player "gfx/sprite_player.png" 2 2 NONE 0
IMAGE logo_minnka_img "gfx/logo_minnka.png" BEST ALL_PALETTE

# --- Sound Resources ---
# The WAV resource line below was removed because the sound effect (sfx_ping)
# is now hardcoded as a C array in sound.c due to issues with WAV generation
# in the automated environment. If a valid WAV file were present, the line would be:
#
# WAV sfx_ping_data "sfx/sfx_ping.wav" SNDRATE_AUTO CHANNELS_1
#
# 'sfx_ping_data': C variable name for the PCM structure in resources.h.
# "sfx/sfx_ping.wav": Path to the WAV file.
# SNDRATE_AUTO: Tells rescomp to try and automatically detect the sample rate from the WAV file.
#               Alternatively, specific rates like SNDRATE_8000, SNDRATE_16000 can be used.
# CHANNELS_1: Specifies the number of channels (1 for mono, 2 for stereo).
#             Mega Drive sound hardware is mono, so typically CHANNELS_1 is used for PCM.
#             Rescomp would convert stereo to mono if specified as CHANNELS_1.
#
PCM sfx_ping_data "sfx/sfx_ping.raw" SOUND_RATE_8000
# Other resource types like MUSIC (for XGM/TFM music) could also be defined here.
# Example:
# XGM music_track "music/my_song.xgm" NONE
#
# For more details on rescomp and resource types, refer to the SGDK documentation.
