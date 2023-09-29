#include "freqshift.h"
#include "constants.h"

FrequencyShifter::FrequencyShifter()
	: Node(NodeType::PROCESSOR, "freqshift", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_fs(44100)
{
	paramMap = {
		{"node_id", NODE_ID},
		{"drywet", DRYWET},
		{"moddepth", MODDEPTH},
		{"freq", FREQ},
	};

	inputMap = {
		{"input", INPUT},
		{"modfreq", MODFREQ}
	};

	outputMap = {
		{"output", OUTPUT}
	};

	initIdStrings();
}

void FrequencyShifter::process() noexcept
{
	float in = inputs[INPUT];

	double inc = 1. / m_fs;
	static double accum = 0;

	if (accum >= m_fs) {
		accum = 0;
	}

	float freq = params[FREQ];
	auto modfreq = inputs[MODFREQ];
	auto moddepth = params[MODDEPTH];

	if (modfreq != 0) {
		freq = freq + ((freq * moddepth) * modfreq);
	}

	float out = 0.f;

	float real = hilbert.real(in);
	float imag = hilbert.imaginary(in);

	float realshift = real * sin(2 * M_PI * accum * freq);
	float imagshift = imag * cos(2 * M_PI * accum * freq);

	accum += inc;

	out = realshift - imagshift;

	outputs[OUTPUT] = (params[DRYWET] * out) + (
		(1 - params[DRYWET]) * in);

}


