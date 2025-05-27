#include "math_utils.h"

float lerp(const float a, const float b, float t) {
    t = MIN(MAX(t, 0.0f), 1.0f);
    return a + (b - a) * t;
}
