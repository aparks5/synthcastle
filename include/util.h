#ifndef UTIL_H_
#define UTIL_H_

#include "math.h"

static float semitoneToRatio(float semitone)
{
    semitone = (semitone < -24) ? -24 : semitone;
    semitone = (semitone > 24) ? 24 : semitone;

    return powf(2., (1.f * semitone / 12));
}


static float midiNoteToFreq(int noteVal)
{
    return pow(2.f, (noteVal - 69.f) / 12.f) * 440.f;
}

template <class T>
static T clamp(T val, T min, T max)
{
    val = (val < min) ? min : val;
    val = (val > max) ? max : val;
    return val;
}

template <class T>
static T linearInterpolate(T y1, T y2, T x)
{
    return x * y2 + (1. - x) * y1;
}

/// see MadBrain's post: https://www.kvraudio.com/forum/viewtopic.php?t=195315
/// """
/// Dunno, for soft clip, I hard clip, then use a polynomial to smooth the
/// transition between non clipped and clipped:
/// The f(x) = 1.5x - 0.5x ^ 3 waveshaper is chosen so that the derivate of
/// f(x) is 0 at points where hard clipping sets in.In mathematical terms,
/// f'(1) = 0 and f'(-1) = 0. It will also have the side effect of having a
/// gain of roughly 1.5 in non distorted parts.
/// """
///
static float clip(float input)
{
    if (input > 1)
        input = 1;
    if (input < -1)
        input = -1;
    return (1.5 * input - 0.5 * input * input * input); // Simple f(x) = 1.5x - 0.5x^3 waveshaper
}

#endif // UTIL_H_
