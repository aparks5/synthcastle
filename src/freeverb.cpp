#include "freeverb.h"

Freeverb::Freeverb()
	: Node(NodeType::PROCESSOR, "freeverb", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_sampleRate(44100)
{
	auto fs = m_sampleRate;
	Allpass ap1(fs, 5.1f, 0.5);
	m_allpassFilters.push_back(ap1);
	Allpass ap2(fs, 12.6f, 0.5);
	m_allpassFilters.push_back(ap2);
	Allpass ap3(fs, 10.f, 0.5);
	m_allpassFilters.push_back(ap3);
	Allpass ap4(fs, 7.7f, 0.5);
	m_allpassFilters.push_back(ap4);

	float fb = 0.84; // tuneable?
	Comb c1(fs, 35.3, fb);
	Comb c2(fs, 36.6, fb);
	Comb c3(fs, 33.8, fb);
	Comb c4(fs, 33.2, fb);
	Comb c5(fs, 28.9, fb);
	Comb c6(fs, 30.7, fb);
	Comb c7(fs, 26.9, fb);
	Comb c8(fs, 25.3, fb);

	m_combFilters.push_back(c1);
	m_combFilters.push_back(c2);
	m_combFilters.push_back(c3);
	m_combFilters.push_back(c4);
	m_combFilters.push_back(c5);
	m_combFilters.push_back(c6);
	m_combFilters.push_back(c7);
	m_combFilters.push_back(c8);
}


void Freeverb::process() noexcept
{
	float in = inputs[INPUT];

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
	outputs[OUTPUT] = (params[WETDRY] * output) + ((1 - params[WETDRY]) * in);
}
