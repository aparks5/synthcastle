/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "sine.h" 
#include "constants.h"
#include "stdio.h"
#include <cmath>
#include <iostream>

constexpr auto NUM_SECONDS = (2);

Sine::Sine()
	: m_freq(125)
	, m_phase(0.0f)
	, m_increment(0.0f)
{
	// initialize look-up table
	for (auto i = 0; i < TABLE_SIZE; i++) {
		m_sine[i] = static_cast<float>(std::sin((i * 1.0f / TABLE_SIZE) * M_PI * 2.0f));
	}

	update();
}

void Sine::reset()
{
	m_increment = 0.0f;
}

void Sine::update()
{
	m_increment = (TABLE_SIZE * m_freq) / SAMPLE_RATE * 1.0f;
}

float Sine::generate()
{
	m_phase += m_increment;

	if (m_phase >= TABLE_SIZE) {
		m_phase -= TABLE_SIZE;
	}

	auto idx = (m_phase) > 0 ? static_cast<size_t>(m_phase) : 0;

	return m_sine[idx];
}
