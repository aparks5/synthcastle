#ifndef UTIL_H_
#define UTIL_H_

#include "math.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

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

static int floatTodB(float f) 
{
	return 20 * log10(f);
}

static float dBtoFloat(int db) 
{
	return pow(10, db / 20.f);
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

template <class T>
static T linearInterpolate(T y1, T y2, T x)
{
    return x * y2 + (1. - x) * y1;
}

// https://stackoverflow.com/questions/1125666/how-do-you-do-bicubic-or-other-non-linear-interpolation-of-re-sampled-audio-da
template <class T>
static T cubicInterpolate(T x0, T x1, T x2, T x3, T t)
{
    double a0, a1, a2, a3;

    a0 = x3 - x2 - x0 + x1;
    a1 = x0 - x1 - a0;
    a2 = x2 - x0;
    a3 = x1;

    return a0 * (t*t*t) + a1 * (t*t) + a2 * t + a3;
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


static std::vector<std::string> tokenize(std::string input, char delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    for (std::string each; std::getline(stream, each, delimiter); tokens.push_back(each));

    return tokens;
}


#endif // UTIL_H_
