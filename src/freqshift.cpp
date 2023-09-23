#include "freqshift.h"
#include "constants.h"

FrequencyShifter::FrequencyShifter()
	: Node(NodeType::FREQ_SHIFT, 0.f, NUM_PARAMS)
	, m_fs(44100)
{
}

float FrequencyShifter::process()
{
	double inc = 1. / m_fs;
	static double accum = 0;
	if (accum >= m_fs) {
		accum = 0;
	}
	auto in = params[INPUT];
	float freq = params[FREQ];
	float out = 0.f;

	float real = hilbert.real(in);
	float imag = hilbert.imaginary(in);

	float realshift = real * sin(2 * M_PI * accum * freq);
	float imagshift = imag * cos(2 * M_PI * accum * freq);

	accum += inc;

	out = realshift - imagshift;
	params[OUTPUT] = (params[DRYWET] * out) + (
		(1 - params[DRYWET]) * in);

	return 0;

}


