#ifndef _TIMER_H_
#define _TIMER_H_

typedef struct Timer {
    float start_time;
    float current_time;
    float elapsed_time;
    float target_time;
    int active;
    void (*on_finished)(void*);
    void *data_ptr;
} Timer;

Timer timer_init(void (*on_finished)(void* data_ptr), void *data_ptr);
void timer_start(Timer *timer, float target_time);
void timer_update(Timer *timer);

#endif
