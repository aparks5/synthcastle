#pragma once

#include "waveform.h"
#include <random>

class SampleAndHold : public WaveForm 
{
public:
    SampleAndHold(int sampleRate);
    float process() override;
    void update(float freq) override;
private:
    float m_counter;
    float m_val;
	std::random_device m_rd; // obtain a random number from hardware
	std::mt19937 m_gen; // seed the generator
	std::uniform_int_distribution<> m_distr; // define the range

};

