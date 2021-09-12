#ifndef UTIL_H_
#define UTIL_H_

#include "math.h"

static float semitoneToRatio(float semitone)
{
    semitone = (semitone < -24) ? -24 : semitone;
    semitone = (semitone > 24) ? 24 : semitone;

    return powf(2., (1.f * semitone / 12));
}

template <class T>
static T clamp(T val, T min, T max)
{
    val = (val < min) ? min : val;
    val = (val > max) ? max : val;
}

template <class T>
static T linearInterpolate(T y1, T y2, T x)
{
    return x * y2 + (1 - x) * y1;
}

#endif // UTIL_H_
