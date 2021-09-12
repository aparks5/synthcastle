#ifndef UTIL_H_
#define UTIL_H_

#include "math.h"

static float semitoneToRatio(float semitone)
{
    semitone = (semitone < -24) ? -24 : semitone;
    semitone = (semitone > 24) ? 24 : semitone;

    return powf(2., (1.f * semitone / 12));
}

#endif // UTIL_H_
