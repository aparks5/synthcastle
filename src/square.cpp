/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "square.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"

#include <iostream>

Square::Square(int id)
	: Oscillator(id)
{
}

void Square::update()
{
	auto samplesPerCycle = m_sampleRate / params[Oscillator::FREQ];
	m_step = 2.0f / samplesPerCycle;
}

float Square::process()
{
	if (m_out >= 1.0f) {
		m_out = -1;
	}

	m_out += m_step;

	return (m_out > 0.0f) ? 1.0f : -1.0f;
}

