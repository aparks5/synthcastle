/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "saw.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"

#include <iostream>


Saw::Saw()
	: m_targetFreq(220)
	, m_output(-1.0f)
	, m_increment(0.0f)
{
	update();
}

void Saw::update()
{
	auto samplesPerCycle = SAMPLE_RATE * 1.0f / m_targetFreq;
	m_increment = 2.0f / samplesPerCycle;
}

float Saw::generate()
{
	if (m_output >= 1.0f) {
		m_output = -1.0f;
	}

	m_output += m_increment;

	return m_output;
}

