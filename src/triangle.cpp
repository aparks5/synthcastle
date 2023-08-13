/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "triangle.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"

#include <iostream>


Triangle::Triangle(int sampleRate)
	: WaveForm(sampleRate)
	, m_bRising(true)
{
	m_sampleRate = sampleRate;
}

void Triangle::update(float freq, float modfreq, float moddepth) 
{
	if (modfreq != 0) {
		freq = freq + ((freq * moddepth) * modfreq);
	}
	
	auto samplesPerCycle = 1. * m_sampleRate / freq;
	m_step = 4.0f / samplesPerCycle;
}

float Triangle::process()
{
	if ((m_out >= 0.9999f)) {
		m_bRising = false;
	}

	if ((m_out <= -0.9999f)) {
		m_bRising = true;
	}

	if (m_bRising) {
		m_out += m_step;
	}
	else {
		m_out -= m_step;
	}

	return m_out;
}

