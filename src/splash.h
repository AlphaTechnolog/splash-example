#ifndef _SPLASH_H_
#define _SPLASH_H_

#include "timer.h"

typedef struct Splash {
    int active;
    float opacity;
    Timer animation_timer;
} Splash;

Splash splash_init(void);
void splash_start(Splash *splash);
void splash_update(Splash *splash);
void splash_render(Splash *splash);

#endif
