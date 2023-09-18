/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "sine.h" 
#include "constants.h"
#include "stdio.h"
#include <cmath>
#include <iostream>
#include "util.h"

Sine::Sine(int sampleRate)
	: WaveForm(sampleRate)
{
	// initialize look-up table
	for (auto i = 0; i < TABLE_SIZE; i++) {
		m_sine[i] = static_cast<float>(std::sin((i * 1.0f / TABLE_SIZE) * M_PI * 2.0f));
	}
}

void Sine::update(float freq)
{

	if (((TABLE_SIZE * freq) / m_sampleRate * 1.0f) != m_step) {
		m_step = (TABLE_SIZE * freq) / m_sampleRate * 1.0f;
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
