#include "freeverb.h"

Freeverb::Freeverb()
{
	Allpass ap1(5.1f, 0.5);
	m_allpassFilters.push_back(ap1);
	Allpass ap2(12.6f, 0.5);
	m_allpassFilters.push_back(ap2);
	Allpass ap3(10.f, 0.5);
	m_allpassFilters.push_back(ap3);
	Allpass ap4(7.7f, 0.5);
	m_allpassFilters.push_back(ap4);

	float fb = 0.84;
	Comb c1(35.3, fb);
	Comb c2(36.6, fb);
	Comb c3(33.8, fb);
	Comb c4(33.2, fb);
	Comb c5(28.9, fb);
	Comb c6(30.7, fb);
	Comb c7(26.9, fb);
	Comb c8(25.3, fb);

	m_combFilters.push_back(c1);
	m_combFilters.push_back(c2);
	m_combFilters.push_back(c3);
	m_combFilters.push_back(c4);
	m_combFilters.push_back(c5);
	m_combFilters.push_back(c6);
	m_combFilters.push_back(c7);
	m_combFilters.push_back(c8);
}


float Freeverb::operator()(float in)
{
	float dry = in;
	float earlyReflections = 0.f;
	// comb filters in parallel
	for (size_t idx = 0; idx < 8; idx++) {
		earlyReflections += (1.f / sqrt(2.f * 8.f)) * m_combFilters[idx](dry);
	}
	float output = earlyReflections;
	// allpass filters in series
	output = m_allpassFilters[0](output);
	output = m_allpassFilters[1](output);
	output = m_allpassFilters[2](output);
	output = m_allpassFilters[3](output);
	return output;
}
