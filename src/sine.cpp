/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "sine.h" 
#include "constants.h"
#include "stdio.h"
#include <cmath>
#include <iostream>
#include "util.h"

constexpr auto NUM_SECONDS = (2);

Sine::Sine(float fs)
	: Oscillator<float>(fs, 0.f, 0.f, 0.f)
{
	// initialize look-up table
	for (auto i = 0; i < TABLE_SIZE; i++) {
		m_sine[i] = static_cast<float>(std::sin((i * 1.0f / TABLE_SIZE) * M_PI * 2.0f));
	}
}


void Sine::freq(float frequency)
{
	m_freq = frequency;
	m_step = (TABLE_SIZE * m_freq) / m_sampleRate * 1.0f;
}

float Sine::operator()()
{
	m_out += m_step;

	if (m_out >= TABLE_SIZE) {
		m_out -= TABLE_SIZE;
	}


	float integerIdx = 0;
	float fractionalIdx = modff(m_out, &integerIdx);

	int nextIdx = integerIdx + 1;
	if (nextIdx > (TABLE_SIZE - 1)) {
		nextIdx = 0;
	}

	return linearInterpolate(m_sine[static_cast<size_t>(integerIdx)], m_sine[nextIdx], fractionalIdx);
}
