#ifndef _SCENE_H_
#define _SCENE_H_

#include "player.h"

typedef enum Scene {
    SCENE_MAIN
} Scene;

typedef struct MainScene {
    Player player;
} MainScene;

MainScene main_scene_init(void);
void main_scene_update(MainScene *ms);
void main_scene_render(MainScene *ms);

#endif
