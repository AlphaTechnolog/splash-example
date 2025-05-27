#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "config.h"
#include "state.h"

int main(int argc, char *argv[]) {
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(TARGET_FPS);

    gstate = malloc(sizeof(State));
    if (!gstate) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    state_init(gstate);

    while (!WindowShouldClose()) {
        state_update();
        BeginDrawing();
        ClearBackground(BLACK);
        state_render();
        EndDrawing();
    }

    CloseWindow();
    state_scene_destroy();
    free(gstate);

    return 0;
}
