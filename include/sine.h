/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SINE_H_
#define SINE_H_

#include "oscillator.h"

constexpr auto TABLE_SIZE = 2048;

class Sine : public Oscillator<float>
{
public:
    Sine(float fs);
    void freq(float frequency) override;
    float operator()() override;
    float operator()(float in) override { return 0.f; }

private:
	float m_sine[TABLE_SIZE];

};

#endif // SINE_H_

