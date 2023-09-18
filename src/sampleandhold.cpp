/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "sampleandhold.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"
#include <random>

SampleAndHold::SampleAndHold(int sampleRate)
	: WaveForm(sampleRate)
	, m_counter(0.)
	, m_val(0.)
	, m_gen(m_rd())
	, m_distr(0, 128)
{
}

void SampleAndHold::update(float freq)
{
	auto samplesPerCycle = 1. * m_sampleRate / freq;
	m_step = samplesPerCycle;
}

float SampleAndHold::process()
{
	m_counter++;
	if (m_counter > m_step) {
		m_val = m_distr(m_gen) / 128.f;
		m_counter = 0;
	}

	return m_val;
}

