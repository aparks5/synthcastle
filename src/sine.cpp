/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "sine.h" 
#include "constants.h"
#include "stdio.h"
#include <cmath>
#include <iostream>
#include "util.h"

Sine::Sine()
	: Oscillator()
{
	// initialize look-up table
	for (auto i = 0; i < TABLE_SIZE; i++) {
		m_sine[i] = static_cast<float>(std::sin((i * 1.0f / TABLE_SIZE) * M_PI * 2.0f));
	}
}

Sine::Sine(int sampleRate)
	: Oscillator()
{
	m_sampleRate = sampleRate;
	// initialize look-up table
	for (auto i = 0; i < TABLE_SIZE; i++) {
		m_sine[i] = static_cast<float>(std::sin((i * 1.0f / TABLE_SIZE) * M_PI * 2.0f));
	}
}


void Sine::update()
{
	float freq = params[Oscillator::FREQ];
	if (params[Oscillator::MODFREQ] != 0) {
		freq = params[Oscillator::FREQ] + (50 * params[Oscillator::MODFREQ]);
	}
	
	if (((TABLE_SIZE * freq) / m_sampleRate * 1.0f) != m_step) {
		m_step = (TABLE_SIZE * freq) / m_sampleRate * 1.0f;
		//m_out = 0;
	}
}

float Sine::process()
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
