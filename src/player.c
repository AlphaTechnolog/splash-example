#include <raylib.h>

#include "config.h"
#include "state.h"
#include "math_utils.h"
#include "player.h"

#define PLAYER_SPEED 40

Player player_init(void) {
    Player p = {0};
    p.speed = 2.5f;
    p.boundaries.width = p.boundaries.height = PLAYER_SPEED;
    p.boundaries.x = (WIDTH - p.boundaries.width) / 2;
    p.boundaries.y = (HEIGHT - p.boundaries.height) / 2;
    p.color = WHITE;
    return p;
}

void player_update(Player *p) {
    switch (blocked_input_reason()) {
        case BLOCKED_INPUT_REASON_TIMER_ACTIVE:
            return;
        default:
            break;
    }

    if (IsKeyDown(KEY_A)) p->boundaries.x -= p->speed;
    if (IsKeyDown(KEY_D)) p->boundaries.x += p->speed;
    if (IsKeyDown(KEY_W)) p->boundaries.y -= p->speed;
    if (IsKeyDown(KEY_S)) p->boundaries.y += p->speed;

    p->boundaries.x = MIN(MAX(p->boundaries.x, 0), WIDTH - p->boundaries.width);
    p->boundaries.y = MIN(MAX(p->boundaries.y, 0), HEIGHT - p->boundaries.height);
}

void player_render(Player *p) {
    DrawRectangle(p->boundaries.x, p->boundaries.y,
            p->boundaries.width, p->boundaries.height,
            p->color);
}
