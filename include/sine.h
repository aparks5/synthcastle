/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SINE_H_
#define SINE_H_

#include "waveform.h"

constexpr size_t TABLE_SIZE = 2048;

class Sine : public WaveForm
{
public:
    Sine(int sampleRate);
    float process() override;
	void update(float freq, float modfreq, float moddepth) override;

private:
	float m_sine[TABLE_SIZE];
};


#endif // SINE_H_

