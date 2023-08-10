/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "saw.h" 

Saw::Saw(int id)
	: Oscillator(id)
{
}

void Saw::update()
{
	auto samplesPerCycle = m_sampleRate * 1.0f / params[Oscillator::FREQ];
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

