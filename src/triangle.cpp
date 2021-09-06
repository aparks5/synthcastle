/// Copyright (c) 2021. Anthony Parks. All rights reserved.
#include "triangle.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"

#include <iostream>

Triangle::Triangle()
	: m_freq(220)
    , m_output(-1.0f)
	, m_increment(0)
	, m_incrementBase(0)
{
	update();
}

void Triangle::reset()
{
	m_increment = 0.f;
}

void Triangle::update()
{
	auto samplesPerCycle = SAMPLE_RATE / m_freq;
	m_incrementBase = 4.0f / samplesPerCycle;
}

float Triangle::generate()
{
		if ((m_output >= 0.99f)) {
			m_increment = -1 * m_incrementBase;
		}

		if ((m_output <= -0.99f)) {
			m_increment = m_incrementBase;
		}

		m_output += m_increment;

		return m_output;
}

