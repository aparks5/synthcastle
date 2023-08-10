/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "triangle.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"

#include <iostream>


Triangle::Triangle(int id)
	: Oscillator(id)
	, m_bRising(true)
{
}

void Triangle::update()
{
	auto samplesPerCycle = 1. * m_sampleRate / params[Oscillator::FREQ];
	m_step = 4.0f / samplesPerCycle;
}

float Triangle::process()
{
	if ((m_out >= 0.9999f)) {
		m_bRising = false;
	}

	if ((m_out <= -0.9999f)) {
		m_bRising = true;
	}

	if (m_bRising) {
		m_out += m_step;
	}
	else {
		m_out -= m_step;
	}

	return m_out;
}

