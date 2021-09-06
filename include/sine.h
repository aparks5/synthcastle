/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SINE_H_
#define SINE_H_

#include "portaudio.h"

constexpr auto TABLE_SIZE = (2048);

class Sine
{
public:
    Sine();
    void reset();
    virtual ~Sine() {};
    void freq(float frequency) { m_freq = frequency; }
    void update();
    float generate();

private:
	float m_sine[TABLE_SIZE];
    float m_freq;
    float m_phase;
    float m_increment;


};

#endif // SINE_H_

