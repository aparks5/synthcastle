#include "quadmixer.h"
#include "imnodes.h"
#include "util.h"

QuadMixer::QuadMixer()
	: Node(NodeType::PROCESSOR, "quadmix", NUM_INPUTS, NUM_PARAMS, NUM_OUTPUTS)
{
	paramMap = {
		{"inputa_id", INPUT_A_ID},
		{"inputb_id", INPUT_B_ID},
		{"inputc_id", INPUT_C_ID},
		{"inputd_id", INPUT_D_ID},
		{"inputa", INPUT_A},
		{"inputb", INPUT_B},
		{"inputc", INPUT_C},
		{"inputd", INPUT_D},
		{"gain_a", GAIN_A},
		{"gain_b", GAIN_B},
		{"gain_c", GAIN_C},
		{"gain_d", GAIN_D},
	};

}

void QuadMixer::process() noexcept 
{
	auto gaina = dBtoFloat(params[GAIN_A]);
	auto gainb = dBtoFloat(params[GAIN_B]);
	auto gainc = dBtoFloat(params[GAIN_C]);
	auto gaind = dBtoFloat(params[GAIN_D]);

	outputs[OUTPUT] = (0.25f * (
		(gaina * params[INPUT_A]) +
		(gainb * params[INPUT_B]) +
		(gainc * params[INPUT_C]) +
		(gaind * params[INPUT_D])));
}
