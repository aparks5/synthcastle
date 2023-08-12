/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SINE_H_
#define SINE_H_

#include "oscillator.h"

constexpr auto TABLE_SIZE = 2048;

class Sine : public Oscillator
{
public:
    Sine();
    Sine(int sampleRate);
    float process() override;
    float process(float in) override { return 0.f; }
    void update() override;

private:
	float m_sine[TABLE_SIZE];

};

#endif // SINE_H_

