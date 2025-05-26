#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#define WIDTH 800
#define HEIGHT 600
#define TITLE "WINDOW"
#define TARGET_FPS 120

#define BACKGROUND_COLOR(color) DrawRectangle(0, 0, WIDTH, HEIGHT, (color));
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static float lerp(float a, float b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return a + (b - a) * t;
}

typedef struct Timer {
    float start_time;
    float current_time;
    float target_time;
    int active;
    void (*on_finished)(void*);
    void *data_ptr;
} Timer;

static Timer timer_init(void (*on_finished)(void *data_ptr), void *data_ptr) {
    Timer t = {0};
    t.active = 0;
    t.target_time = 0;
    t.start_time = 0.0f;
    t.current_time = 0.0f;
    t.on_finished = on_finished;
    t.data_ptr = data_ptr;
    return t;
}

static void timer_start(Timer *timer, float target_time) {
    timer->active = 1;
    timer->start_time = GetTime();
    timer->current_time = GetTime();
    timer->target_time = target_time;
}

static void timer_update(Timer *timer) {
    if (timer->active == 0) return;
    timer->current_time = GetTime();
    if (timer->current_time - timer->start_time >= timer->target_time) {
        timer->active = 0;
        timer->on_finished(timer->data_ptr);
    }
}

typedef struct Splash {
    int active;
    float fade_start_time;
    float opacity;
} Splash;

static Splash splash_init(void) {
    Splash splash = {0};
    splash.active = 0;
    splash.fade_start_time = 0.0f;
    splash.opacity = 1.0f;
    return splash;
}

static void splash_start(Splash *splash) {
    splash->active = 1;
    splash->fade_start_time = GetTime();
    splash->opacity = 1.0f;
}

static void splash_update(Splash *splash) {
    if (splash->active == 0) return;

    double current_time = GetTime();
    double elapsed_time = current_time - splash->fade_start_time;

    float fade_duration = 2.0f;  // 2 seconds.
    float initial_opacity = 1.0f;
    float final_opacity = 0.0f;
    float t = (float)(elapsed_time / fade_duration);

    splash->opacity = lerp(initial_opacity, final_opacity, t);

    if (elapsed_time >= fade_duration) {
        splash->active = 0;
        splash->opacity = final_opacity;
    }
}

static void splash_render(Splash *splash) {
    BACKGROUND_COLOR(((Color){
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255 * splash->opacity,
    }));
}

typedef enum Scene {
    SCENE_MAIN
} Scene;

typedef struct State {
    Scene active_scene;
    Splash splash;
    void *scene_ptr;
    Timer start_splash_fadeout_timer;
} State;

State game_state;

typedef struct Player {
    Rectangle boundaries;
    Color color;
    float speed;
} Player;

static Player player_init(void) {
    Player p = {0};
    p.speed = 2.5f;
    p.boundaries.width = p.boundaries.height = 40;
    p.boundaries.x = (WIDTH - p.boundaries.width) / 2;
    p.boundaries.y = (HEIGHT - p.boundaries.height) / 2;
    p.color = WHITE;
    return p;
}

static int can_use_input(void) {
    int is_splash_active = game_state.splash.active == 1;
    int is_fadeout_active = game_state.start_splash_fadeout_timer.active == 1;
    return is_splash_active == 0 && is_fadeout_active == 0;
}

typedef enum BlockedInputReason {
    BLOCKED_INPUT_REASON_TIMER_ACTIVE,
    BLOCKED_INPUT_REASON_SPLASH,
    BLOCKED_INPUT_REASON_NONE,
} BlockedInputReason;

static BlockedInputReason blocked_input_reason(void) {
    int is_splash_active = game_state.splash.active == 1;
    int is_fadeout_active = game_state.start_splash_fadeout_timer.active == 1;
    if (is_splash_active) return BLOCKED_INPUT_REASON_SPLASH;
    if (is_fadeout_active) return BLOCKED_INPUT_REASON_TIMER_ACTIVE;
    return BLOCKED_INPUT_REASON_NONE;
}

static void player_update(Player *p) {
    if (!can_use_input()) {
        switch (blocked_input_reason()) {
            case BLOCKED_INPUT_REASON_TIMER_ACTIVE:
                return;
            default:
                break;
        }
    }

    const float speed = p->speed;
    if (IsKeyDown(KEY_A)) p->boundaries.x -= speed;
    if (IsKeyDown(KEY_D)) p->boundaries.x += speed;
    if (IsKeyDown(KEY_W)) p->boundaries.y -= speed;
    if (IsKeyDown(KEY_S)) p->boundaries.y += speed;

    // clamp values to the edges of the screen
    p->boundaries.x = MIN(MAX(p->boundaries.x, 0), WIDTH - p->boundaries.width);
    p->boundaries.y = MIN(MAX(p->boundaries.y, 0), HEIGHT - p->boundaries.height);
}

static void player_render(Player *p) {
    DrawRectangle(p->boundaries.x, p->boundaries.y,
            p->boundaries.width, p->boundaries.height,
            p->color);
}

typedef struct MainScene {
    Player player;
} MainScene;

static MainScene main_scene_init(void) {
    MainScene s = {0};
    s.player = player_init();
    return s;
}

static void main_scene_update(MainScene *ms) {
    player_update(&ms->player);
}

static void main_scene_render(MainScene *ms) {
    BACKGROUND_COLOR(GetColor(0x3832a8ff));
    player_render(&ms->player);
}

static void on_splash_fadeout_notify(void *_) {
    splash_start(&game_state.splash);
}

static State state_init(void) {
    State s = {0};
    s.splash = splash_init();
    s.start_splash_fadeout_timer = timer_init(on_splash_fadeout_notify, NULL);
    s.active_scene = SCENE_MAIN;

    MainScene scene = main_scene_init();
    s.scene_ptr = malloc(sizeof(MainScene));
    if (!s.scene_ptr) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy((void*)s.scene_ptr, (const void*)&scene, sizeof(MainScene));

    // 2 seconds of wait before start fadeout
    timer_start(&s.start_splash_fadeout_timer, 2.0f);

    return s;
}

static void current_scene_update(State *state) {
    switch (state->active_scene) {
        case SCENE_MAIN:
            main_scene_update((MainScene*)state->scene_ptr);
            break;
    }
}

static void current_scene_render(State *state) {
    switch (state->active_scene) {
        case SCENE_MAIN:
            main_scene_render((MainScene*)state->scene_ptr);
            break;
    }
}

static void state_destroy_scene(State *state) {
    free(state->scene_ptr);
}

static void state_update(State *state) {
    timer_update(&state->start_splash_fadeout_timer);
    splash_update(&state->splash);
    current_scene_update(state);
}

static void state_render(State *state) {
    current_scene_render(state);
    splash_render(&state->splash);
}

int main(int argc, char *argv[]) {
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(TARGET_FPS);

    game_state = state_init();

    while (!WindowShouldClose()) {
        state_update(&game_state);
        BeginDrawing();
        ClearBackground(BLACK);
        state_render(&game_state);
        EndDrawing();
    }

    CloseWindow();

    state_destroy_scene(&game_state);

    return 0;
}
