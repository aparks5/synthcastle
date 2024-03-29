#include "gain.h"
#include "util.h"
#include "math.h"
#include "constants.h"

Gain::Gain()
    : Node(NodeType::PROCESSOR, "gain", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	params[GAIN] = 1.f;

	paramMap = {
			{"gain", GAIN},
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
		{"left", OUTPUT_LEFT},
		{"right", OUTPUT_RIGHT}
	};

	initIdStrings();
}

void Gain::setGaindB(float gaindB)
{
	m_gain = dBtoFloat(gaindB);
}

void Gain::process() noexcept
{
	float in = inputs[INPUT];
	auto g = dBtoFloat(params[GAIN]);

	in *= g;

	if (inputs[GAINMOD] != 0) {
		in *= inputs[GAINMOD];
		if (params[GAINMOD_DEPTH] != 0) {
			in *= params[GAINMOD_DEPTH];
		}
	}

	auto p = params[PAN];

	if (params[PANMOD] != 0) {
		p *= inputs[PANMOD];
		if (params[PANMOD_DEPTH] != 0) {
			p *= params[PANMOD_DEPTH];
		}
	}

	float pan = M_PI * 0.25 * p;
    float scale = 0.7071; // ~= sqrt(2)/2
	float rightPanGain = scale * (cos(pan) + sin(pan));
	float leftPanGain = scale * (cos(pan) - sin(pan));

	outputs[OUTPUT_LEFT] = in * leftPanGain;
	outputs[OUTPUT_RIGHT] = in * rightPanGain;
}
