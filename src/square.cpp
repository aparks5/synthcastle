/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "square.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"

#include <iostream>

Square::Square(int sampleRate)
	: WaveForm(sampleRate)
{
}

void Square::update(float freq, float modfreq, float moddepth)
{
	if (modfreq != 0) {
		freq = freq + ((freq * moddepth) * modfreq);
	}

	auto samplesPerCycle = m_sampleRate / freq;
	m_step = 2.0f / samplesPerCycle;
}

float Square::process()
{
	if (m_out >= 1.0f) {
		m_out = -1;
	}

	m_out += m_step;

	return (m_out > 0.0f) ? 1.0f : -1.0f;
}

