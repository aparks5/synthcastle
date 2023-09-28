#include "freqshift.h"
#include "constants.h"

FrequencyShifter::FrequencyShifter()
	: Node(NodeType::PROCESSOR, "freqshift", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_fs(44100)
{
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


