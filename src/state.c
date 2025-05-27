#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scene.h"
#include "splash.h"
#include "timer.h"
#include "state.h"

// two seconds for fade start
#define FADESTART_TIME 2.0f

BlockedInputReason blocked_input_reason(void) {
    int is_splash_active = gstate->splash.active == 1;
    int is_fadeout_active = gstate->start_splash_fadeout_timer.active == 1;
    if (is_splash_active) return BLOCKED_INPUT_REASON_SPLASH;
    if (is_fadeout_active) return BLOCKED_INPUT_REASON_TIMER_ACTIVE;
    return BLOCKED_INPUT_REASON_NONE;
}

static void on_splash_fadeout_notify(void *_) {
    splash_start(&gstate->splash);
}

void state_init(State *ptr) {
    ptr->splash = splash_init();
    ptr->start_splash_fadeout_timer = timer_init(on_splash_fadeout_notify, NULL);
    ptr->active_scene = SCENE_MAIN;

    MainScene scene = main_scene_init();
    ptr->scene_ptr = malloc(sizeof(MainScene));
    if (!ptr->scene_ptr) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy((void*)ptr->scene_ptr, (const void*)&scene, sizeof(MainScene));

    timer_start(&ptr->start_splash_fadeout_timer, FADESTART_TIME);
}

void state_current_scene_update() {
    switch (gstate->active_scene) {
        case SCENE_MAIN:
            main_scene_update((MainScene*)gstate->scene_ptr);
            break;
    }
}

void state_current_scene_render() {
    switch (gstate->active_scene) {
        case SCENE_MAIN:
            main_scene_render((MainScene*)gstate->scene_ptr);
            break;
    }
}

void state_scene_destroy() {
    free(gstate->scene_ptr);
}

void state_update() {
    timer_update(&gstate->start_splash_fadeout_timer);
    splash_update(&gstate->splash);
    state_current_scene_update();
}

void state_render() {
    state_current_scene_render();
    splash_render(&gstate->splash);
}
