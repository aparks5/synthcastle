#include "fdn.h"
#include "constants.h"

FeedbackDelayNetwork::FeedbackDelayNetwork(size_t order)
	: m_lowpassDelayElements{}
{
	if (order > 8) {
		order = 8;
	}

	for (size_t idx = 0; idx < order; idx++) {
		Delay delay(SAMPLE_RATE, 1.f);
		m_delays.push_back(delay);
		m_delays[idx].update(m_primeTimes[idx], 0.0f); // feedback comes from write()
		m_delayOutputs.push_back(0.f);
		m_lowpassDelayElements[idx] = 0.f;
	}
}

void FeedbackDelayNetwork::reset()
{
	for (auto d : m_delays) {
		d.reset();
	}
}

float FeedbackDelayNetwork::operator()(float in)
{
	float out = 0.f;
	for (size_t idx = 0; idx < m_delays.size(); idx++) {
		float accum = 0.f;
		for (int jdx = 0; jdx < m_delays.size(); jdx++) {
			accum += 0.95f * m_scaleFactor * m_hadamard[idx][jdx] * m_lowpassDelayElements[jdx];
		}
		out += accum;
		accum += in;

		m_lowpassDelayElements[idx] = (m_delays[idx]()*0.8) + (0.2 * m_lowpassDelayElements[idx]);
		m_delays[idx].write(accum);
	}

	return out;

}