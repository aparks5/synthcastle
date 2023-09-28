#include "gain.h"
#include "util.h"
#include "math.h"
#include "constants.h"

Gain::Gain()
    : Node(NodeType::GAIN, "gain", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	params[GAIN] = 1.f;
	paramMap = {
			{"gain", GAIN},
			{"input_id", INPUT_ID},
			{"left_id", LEFT_ID},
			{"right_id", RIGHT_ID},
			{"panmod_id", PANMOD_ID},
			{"gainmod_depth", GAINMOD_DEPTH},
			{"pan", PAN},
			{"panmod_depth", PANMOD_DEPTH},
	};

	inputMap = {
		{"input", INPUT},
		{"gainmod", GAINMOD},
		{"panmod", PANMOD},
	};

	outputMap = {
		{"output_left", OUTPUT_LEFT},
		{"output_right", OUTPUT_RIGHT}
	};

	// these could be auto generated from keys of inputMap + id
	inputIdStrings = {
		"input_id",
		"gainmod_id",
		"panmod_id"
	};
	
	outputIdStrings = {
		"output_left_id",
		"output_right_id"
	};

}

void Gain::setGaindB(float gaindB)
{
	m_gain = dBtoFloat(gaindB);
}

void Gain::process() noexcept
{
	float in = inputs[INPUT];

	auto g = dBtoFloat(params[GAIN]);
	in *= g * inputs[GAINMOD];

	auto p = params[PAN];

	if (params[PANMOD] != 0) {
		p *= inputs[PANMOD];
		if (params[PANMOD_DEPTH] != 0) {
			p *= params[PANMOD_DEPTH];
		}
	}

	float temp = M_PI * 0.25 * p;

    float scale = 0.7071; // ~= sqrt(2)/2
	float rightPanGain = scale * (cos(temp) + sin(temp));
	float leftPanGain = scale * (cos(temp) - sin(temp));

	outputs[OUTPUT_LEFT] = in * leftPanGain;
	outputs[OUTPUT_RIGHT] = in * rightPanGain;
}
