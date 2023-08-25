#include "filter.h"

#include "imnodes.h"

Filter::Filter()
	: Node(NodeType::FILTER, 0.f, NUM_PARAMS)
	, m_moogFilter(44100)
{
}

int Filter::lookupParam(std::string str) {
	return m_lookup[str];
}

float Filter::process(float in)
{
	m_moogFilter.q(params[FiltParams::Q]);
	auto freq = params[FiltParams::FREQ] +
		(params[FiltParams::FREQ]
			* params[FiltParams::FREQMOD]
			* params[FiltParams::MODDEPTH]);
	m_moogFilter.freq(freq);
	float out = in;
	m_moogFilter.apply(&out, 1);
	return out;
}


