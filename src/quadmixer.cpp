#include "quadmixer.h"
#include "imnodes.h"
#include "util.h"

QuadMixer::QuadMixer()
	: Node(NodeType::PROCESSOR, "mixer", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	paramMap = {
		{"gain_a", GAIN_A},
		{"gain_b", GAIN_B},
		{"gain_c", GAIN_C},
		{"gain_d", GAIN_D},
	};

	inputMap = {
		{"inputa", INPUT_A},
		{"inputb", INPUT_B},
		{"inputc", INPUT_C},
		{"inputd", INPUT_D},
	};

	outputMap = {
		{"output", OUTPUT}
	};

	initIdStrings();

}

void QuadMixer::process() noexcept 
{
	auto gaina = dBtoFloat(params[GAIN_A]);
	auto gainb = dBtoFloat(params[GAIN_B]);
	auto gainc = dBtoFloat(params[GAIN_C]);
	auto gaind = dBtoFloat(params[GAIN_D]);

	outputs[OUTPUT] = (0.25f * (
		(gaina * inputs[INPUT_A]) +
		(gainb * inputs[INPUT_B]) +
		(gainc * inputs[INPUT_C]) +
		(gaind * inputs[INPUT_D])));
}
