#include "gain.h"
#include "util.h"

Gain::Gain()
    : Node(NodeType::GAIN, 1., NUM_PARAMS)
{
	params[Gain::GAIN] = 1.f;
}

int Gain::lookupParam(std::string str) {
	return m_lookup[str];
}

void Gain::setGaindB(float gaindB)
{
	m_gain = dBtoFloat(gaindB);
}

float Gain::process(float in)
{
	auto g = dBtoFloat(params[Gain::GAIN]);
	return in * g * params[Gain::GAINMOD];
}
