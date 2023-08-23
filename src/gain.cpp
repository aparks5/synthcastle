#include "gain.h"

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

int Gain::floatTodB(float f) const
{
	return 20 * log10(f);
}

float Gain::dBtoFloat(int db) const
{
	return pow(10, db / 20.f);
}

float Gain::process(float in)
{
	return in * params[Gain::GAIN] * params[Gain::GAINMOD];
}
