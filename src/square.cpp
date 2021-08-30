/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "square.h" 
#include "stdio.h"
#include "math.h"

#include <iostream>

constexpr auto NUM_SECONDS = (2);
constexpr auto SAMPLE_RATE = (44100);
constexpr auto FRAMES_PER_BUFFER = (256);

Square::Square()
	: m_targetFreq(220)
    , m_output(-1)
	, m_increment(0)
{
	update();
}

void Square::update()
{
	auto samplesPerCycle = SAMPLE_RATE / m_targetFreq;
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

