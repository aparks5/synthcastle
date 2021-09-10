/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "saw.h" 

Saw::Saw(float fs)
	: Oscillator<float>(fs, 0.f, 0.f, 0.f)
{
}

void Saw::freq(float frequency)
{
	m_freq = frequency;
	auto samplesPerCycle = m_fs * 1.0f / m_freq;
	m_step = 2.0f / samplesPerCycle;
}

float Saw::operator()()
{
	if (m_out>= 1.0f) {
		m_out = -1.0f;
	}

	m_out += m_step;

	return m_out;
}

