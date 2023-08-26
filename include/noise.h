#pragma once

#include "waveform.h"
#include <random>

class Noise : public WaveForm 
{
public:
    Noise(int sampleRate);
	void update(float freq) override;
    float process() override;

private:
	bool m_bQuiet;
	std::random_device m_rd; // obtain a random number from hardware
	std::mt19937 m_gen; // seed the generator
	std::uniform_int_distribution<> m_distr; // define the range

};

