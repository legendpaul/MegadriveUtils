#include "music.h"
#include <xgm.h> // SGDK's XGM driver header

void music_init_driver() {
    XGM_init();
}

void music_start(u8 xgm_track_res_id) {
    // Check if XGM is already playing something, stop it first if needed.
    if (XGM_isPlaying()) {
        XGM_stopPlay();
    }
    XGM_startPlay(xgm_track_res_id);
}

void music_stop() {
    XGM_stopPlay();
}

void music_update_driver() {
    XGM_update();
}
