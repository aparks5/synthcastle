#include "quadmixer.h"
#include "imnodes.h"
#include "util.h"

QuadMixer::QuadMixer()
	: Node(NodeType::QUAD_MIXER, 0.f, NUM_PARAMS)
{}

int QuadMixer::lookupParam(std::string str)
{
	return m_lookup[str];
}

float QuadMixer::process()
{
	auto gaina = dBtoFloat(params[GAIN_A]);
	auto gainb = dBtoFloat(params[GAIN_B]);
	auto gainc = dBtoFloat(params[GAIN_C]);
	auto gaind = dBtoFloat(params[GAIN_D]);

	return (0.25f * (
		(gaina * params[INPUT_A]) +
		(gainb * params[INPUT_B]) +
		(gainc * params[INPUT_C]) +
		(gaind * params[INPUT_D])));
}
