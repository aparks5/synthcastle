#include "quadmixer.h"
#include "imnodes.h"

QuadMixer::QuadMixer()
	: Node(NodeType::QUAD_MIXER, 0.f, NUM_PARAMS)
{}

int QuadMixer::lookupParam(std::string str)
{
	return m_lookup[str];
}

float QuadMixer::process()
{
	return (0.25f * (params[INPUT_A] + params[INPUT_B] + params[INPUT_C] + params[INPUT_D]));
}
