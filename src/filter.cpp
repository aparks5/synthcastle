#include "filter.h"

#include "imnodes.h"

Filter::Filter()
	: Node(NodeType::PROCESSOR, "filter", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, moog(44100)
{
	paramMap = {
		{"node_id", NODE_ID},
		{"freq", FREQ},
		{"filter_type", FILTER_TYPE},
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

	float in = inputs[INPUT];

	auto freq = params[FREQ];

	if (inputs[FREQMOD] != 0) {
		freq += (inputs[FREQMOD] * inputs[MODDEPTH] * freq);
	}

	filterType = static_cast<FilterType>(params[FILTER_TYPE]);

	switch (filterType)
	{
	case MOOG:
	{
		moog.SetResonance(params[Q]);
		moog.SetCutoff(freq);
		moog.Process(&in, 1);
	}
	break;
	case DIODE:
	{
		diode.setResonance(params[Q]);
		diode.setCutoff(freq);
		in = diode.Tick(in);
	}
	break;
	}

	outputs[OUTPUT] = in;
}


