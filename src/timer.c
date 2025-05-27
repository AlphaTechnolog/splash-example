#include <raylib.h>

#include "timer.h"

Timer timer_init(void (*on_finished)(void *data_ptr), void *data_ptr) {
    Timer t = {0};
    t.active = 0;
    t.target_time = 0;
    t.start_time = 0.0f;
    t.current_time = 0.0f;
    t.on_finished = on_finished;
    t.data_ptr = data_ptr;
    return t;
}

void timer_start(Timer *timer, float target_time) {
    timer->active = 1;
    timer->start_time = timer->current_time = GetTime();
    timer->elapsed_time = 0.0f;
    timer->target_time = target_time;
}

void timer_update(Timer *timer) {
    if (timer->active == 0) return;

    timer->current_time = GetTime();
    if ((timer->elapsed_time = timer->current_time - timer->start_time) >= timer->target_time) {
        timer->active = 0;
        timer->on_finished(timer->data_ptr);
    }
}
