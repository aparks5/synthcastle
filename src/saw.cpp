/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "saw.h" 

Saw::Saw(int sampleRate)
	: WaveForm(sampleRate)
{
}

void Saw::update(float freq, float modfreq, float moddepth)
{
	if (modfreq != 0) {
		freq = freq + ((freq * moddepth) * modfreq);
	}

	auto samplesPerCycle = m_sampleRate * 1.0f / freq;
	m_step = 2.0f / samplesPerCycle;
}

float Saw::process()
{
	if (m_out>= 1.0f) {
		m_out = -1.0f;
	}

	m_out += m_step;

	return m_out;
}

