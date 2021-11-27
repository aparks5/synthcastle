/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "triangle.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"

#include <iostream>


Triangle::Triangle(float fs)
	: Oscillator<float>(fs, 0.f, 0.f, 0.f)
	, m_bRising(true)
{
}

void Triangle::freq(float frequency)
{
	m_freq = frequency;
	auto samplesPerCycle = 1. * m_sampleRate / m_freq;
	m_step = 4.0f / samplesPerCycle;
}

float Triangle::operator()()
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

