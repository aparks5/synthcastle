/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "square.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"

#include <iostream>

constexpr auto NUM_SECONDS = (2);

Square::Square(float fs)
	: Oscillator<float>(fs, 0.f, 0.f, 0.f)
{
}

void Square::freq(float frequency)
{
	m_freq = frequency;
	auto samplesPerCycle = SAMPLE_RATE / m_freq;
	m_step = 2.0f / samplesPerCycle;
}

float Square::operator()()
{
	if (m_out >= 1.0f) {
		m_out = -1;
	}

	m_out += m_step;

	return (m_out > 0.0f) ? 1.0f : -1.0f;
}

