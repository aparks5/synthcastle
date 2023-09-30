#include "filter.h"

#include "imnodes.h"

Filter::Filter()
	: Node(NodeType::PROCESSOR, "filter", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_moogFilter(44100)
{
	paramMap = {
		{"node_id", NODE_ID},
		{"freq", FREQ},
		{"q", Q}
	};

	inputMap = {
		{"input", INPUT},
		{"freqmod", FREQMOD},
		{"moddepth", MODDEPTH}
	};

	outputMap = {
		{"output", OUTPUT}
	};

	initIdStrings();
}

void Filter::process() noexcept
{
	m_moogFilter.SetResonance(params[Q]);

	auto freq = params[FREQ] + (params[FREQ] * inputs[FREQMOD] * inputs[MODDEPTH]);

	m_moogFilter.SetCutoff(freq);

	float in = inputs[INPUT];
	m_moogFilter.Process(&in, 1);
	outputs[OUTPUT] = in;
}


