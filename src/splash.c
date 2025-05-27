#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#include "timer.h"
#include "math_utils.h"
#include "macro_utils.h"
#include "config.h"
#include "state.h"
#include "splash.h"

#define INITIAL_OPACITY 1.0f
#define FINAL_OPACITY 0.0f

// duration of 1 second.
#define SPLASH_FADEOUT_DURATION 1.0f

static void on_splash_timer_end(void *_) {
    Splash *splash = &gstate->splash;
    splash->active = 0;
    splash->opacity = FINAL_OPACITY;
}

Splash splash_init(void) {
    Splash s = {0};
    s.active = 0;
    s.opacity = 1.0f;
    s.animation_timer = timer_init(on_splash_timer_end, NULL);
    return s;
}

void splash_start(Splash *splash) {
    splash->active = 1;
    splash->opacity = 1.0f;
    timer_start(&splash->animation_timer, SPLASH_FADEOUT_DURATION);
}

void splash_update(Splash *splash) {
    timer_update(&splash->animation_timer);

    if (splash->active == 0)
        return;

    const float t = (float)(splash->animation_timer.elapsed_time / SPLASH_FADEOUT_DURATION);

    splash->opacity = lerp(INITIAL_OPACITY, FINAL_OPACITY, t);
}

void splash_render(Splash *splash) {
    if (splash->active == 0 && gstate->start_splash_fadeout_timer.active == 0)
        return;

    const char *title = "Splash example";
    const char *author = "https://github.com/AlphaTechnolog/splash-example";
    const float title_fontsize = 42;
    const float author_fontsize = 12;
    const float alpha = 255 * splash->opacity;
    const float titlelength = MeasureText(title, title_fontsize);
    const float authorlength = MeasureText(author, author_fontsize);
    const float spacing = 6;
    const float screen_middle = (HEIGHT - (title_fontsize + author_fontsize + spacing)) / 2;

    const Vector2 titlepos = (Vector2){
        .x = (WIDTH - titlelength) / 2,
        .y = screen_middle - (title_fontsize + spacing / 2),
    };

    const Vector2 authorpos = (Vector2){
        .x = (WIDTH - authorlength) / 2,
        .y = screen_middle + (author_fontsize + spacing / 2),
    };

    const Color background = (Color){0, 0, 0, alpha};
    const Color foreground = (Color){255, 255, 255, alpha};

    BACKGROUND_COLOR(background);
    DrawText(title, titlepos.x, titlepos.y, title_fontsize, foreground);
    DrawText(author, authorpos.x, authorpos.y, author_fontsize, foreground);
}
