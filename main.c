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
    float elapsed_time;
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
    timer->elapsed_time = 0.0f;
    timer->target_time = target_time;
}

static void timer_update(Timer *timer) {
    if (timer->active == 0) return;
    timer->current_time = GetTime();
    if ((timer->elapsed_time = timer->current_time - timer->start_time) >= timer->target_time) {
        timer->active = 0;
        timer->on_finished(timer->data_ptr);
    }
}

typedef struct Splash {
    int active;
    float opacity;
    Timer animation_timer;
} Splash;

static void on_splash_timer_end(void *data_ptr) {
    Splash *splash = (Splash*)data_ptr;
    splash->active = 0;
    splash->opacity = 0.0f;
}

static Splash splash_init(void) {
    Splash splash = {0};
    splash.active = 0;
    splash.opacity = 1.0f;
    splash.animation_timer = timer_init(on_splash_timer_end, (void*)&splash);
    return splash;
}

static void splash_start(Splash *splash) {
    splash->active = 1;
    splash->opacity = 1.0f;
    timer_start(&splash->animation_timer, 1.0f);  // fade of 1 second.
}

static void splash_update(Splash *splash) {
    if (splash->active == 0) return;

    timer_update(&splash->animation_timer);

    float fade_duration = 1.0f;
    float t = (float)(splash->animation_timer.elapsed_time / fade_duration);

    splash->opacity = lerp(1.0f, 0.0f, t);
}

static void splash_render(Splash *splash) {
    const char *title = "Sample game";
    const float fontsize = 42;
    float alpha = 255 * splash->opacity;
    float length = MeasureText(title, fontsize);
    Vector2 textpos = (Vector2){(WIDTH - length) / 2, (HEIGHT - 32.0f) / 2};
    BACKGROUND_COLOR(((Color){0, 0, 0, alpha}));
    DrawText(title, textpos.x, textpos.y, fontsize, (Color){255, 255, 255, alpha});
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
    // not checking splash blocked input reason because i think it may be better for the player
    // to be able to not move only when the timer is active but not when it is fading.
    switch (blocked_input_reason()) {
        case BLOCKED_INPUT_REASON_TIMER_ACTIVE:
            return;
        default:
            break;
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
