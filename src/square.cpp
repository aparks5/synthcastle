/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "square.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"

#include <iostream>

constexpr auto NUM_SECONDS = (2);

Square::Square()
	: m_freq(220)
    , m_output(-1)
	, m_increment(0)
{
	update();
}

void Square::reset()
{
	m_increment = 0;
}

void Square::update()
{
	auto samplesPerCycle = SAMPLE_RATE / m_freq;
	m_increment = 2.0f / samplesPerCycle;
}

float Square::generate()
{
	if (m_output>= 1.0f) {
		m_output = -1;
	}

	m_output += m_increment;

	return (m_output > 0.0f) ? 1.0f : -1.0f;
}

