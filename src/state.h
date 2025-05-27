#ifndef _STATE_H_
#define _STATE_H_

#include "timer.h"
#include "splash.h"
#include "scene.h"

typedef struct State {
    Scene active_scene;
    Splash splash;
    void *scene_ptr;
    Timer start_splash_fadeout_timer;
} State;

typedef enum BlockedInputReason {
    BLOCKED_INPUT_REASON_TIMER_ACTIVE,
    BLOCKED_INPUT_REASON_SPLASH,
    BLOCKED_INPUT_REASON_NONE,
} BlockedInputReason;

State *gstate;

void state_init(State *ptr);
void state_current_scene_update();
void state_current_scene_render();
void state_scene_destroy();
void state_update();
void state_render();
BlockedInputReason blocked_input_reason(void);

#endif
