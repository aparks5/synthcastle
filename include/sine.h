/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SINE_H_
#define SINE_H_

#include "portaudio.h"

constexpr auto TABLE_SIZE = (2048);

class Sine
{
public:
    Sine();
    virtual ~Sine() {};
    void setSampleFreq(float frequency) { m_targetFreq = frequency; }
    void update();
    float generate();

private:
	float m_sine[TABLE_SIZE];
    float m_targetFreq;
    float m_phase;
    float m_increment;


};

#endif // SINE_H_

