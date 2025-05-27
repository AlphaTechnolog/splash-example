#include <raylib.h>

#include "macro_utils.h"
#include "scene.h"

MainScene main_scene_init(void) {
    MainScene s = {0};
    s.player = player_init();
    return s;
}

void main_scene_update(MainScene *ms) {
    player_update(&ms->player);
}

void main_scene_render(MainScene *ms) {
    BACKGROUND_COLOR(GetColor(0x3832a8ff));
    player_render(&ms->player);
}
