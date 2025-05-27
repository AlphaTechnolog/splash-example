#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <raylib.h>

typedef struct Player {
    Rectangle boundaries;
    Color color;
    float speed;
} Player;

Player player_init(void);
void player_update(Player *p);
void player_render(Player *p);

#endif
