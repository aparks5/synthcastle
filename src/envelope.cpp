#include "envelope.h"
#include "math.h"
#include "constants.h"

Envelope::Envelope()
	: m_stage(ATTACK)
	, m_counter(0)
	, m_gain(0)
{
}

void Envelope::setParams(EnvelopeParams params)
{
	m_params = params;
	reset();
}

void Envelope::reset()
{
	m_stage = ATTACK;
	m_counter = 0;
}

// TODO: envelope should just return a value, and then use that to convert to dB or apply to filter cutoff etc.
// as it is written it is only an amplitude envelope
// it should not be coupled to Gain in its class definition
float Envelope::apply(size_t numSamples)
{
	m_counter += numSamples;

	switch (m_stage) {
	case ATTACK:
		if (m_counter > m_params.attackTimeSamps) {
			m_counter = 0;
			m_stage = DECAY;
		}
		else if (m_params.attackTimeSamps != 0) {
			m_gain = static_cast<double>(1.f * m_counter / m_params.attackTimeSamps);
		}
		else {
			m_gain = 0;
		}
	break;
	case DECAY:
		if (m_counter > m_params.decayTimeSamps) {
			m_counter = 0;
			m_stage = SUSTAIN;
		}
		else if (m_params.decayTimeSamps > 0) {
			m_gain = static_cast<float>((1.f * m_counter / m_params.decayTimeSamps) * pow(10, 1.f * m_params.sustainLeveldB / 20));
		}
		else {
			m_gain = pow(10, 1.f * m_params.sustainLeveldB / 20);
		}
		break;
	case SUSTAIN:
        m_gain = pow(10, 1.f * m_params.sustainLeveldB / 20);
		if (!m_bNoteOn) {
			m_counter = 0;
			m_stage = RELEASE;
		}
		break;
	case RELEASE:
		if (m_counter > m_params.releaseTimeSamps) {
			m_gain = 0;
		}
		else if (m_counter < m_params.releaseTimeSamps) {
			auto relGain = static_cast<double>(1.f * m_counter / m_params.releaseTimeSamps);
			m_gain = pow(10, (m_params.sustainLeveldB * 1.f / 20)) - relGain;
		}
		else {
			m_gain = 0;
		}
		break;
	}

	return m_gain;
}
