#include "envelope.h"
#include "math.h"
#include "constants.h"

Envelope::Envelope()
	: m_stage(ATTACK)
	, m_counter(0)
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
float Envelope::apply(float sample)
{
	auto output = sample;
	m_gain.setGaindB(0);

	switch (m_stage) {
	case ATTACK:
		m_counter++;
		if (m_params.attackTimeSamps) {
			m_gain.setGaindB(20 * log10(static_cast<double>(1.f * m_counter / m_params.attackTimeSamps)));
		}
		if (m_counter > m_params.attackTimeSamps) {
			m_counter = 0;
			m_stage = DECAY;
		}
		break;
	case DECAY:
		m_counter++;
		if (m_counter > m_params.decayTimeSamps) {
			m_counter = 0;
			m_stage = SUSTAIN;
		}
		else {
			auto decayGaindB = 0;
			if (m_params.decayTimeSamps > 0) {
				decayGaindB = static_cast<float>((1.f * m_counter / m_params.decayTimeSamps) * m_params.sustainLeveldB);
			}
			m_gain.setGaindB(decayGaindB);
		}
		break;
	case SUSTAIN:
		m_counter++;
		m_gain.setGaindB(m_params.sustainLeveldB);
		if (m_counter > (0.500*SAMPLE_RATE)) {
			m_counter = 0;
			m_stage = RELEASE;
		}
		break;
	case RELEASE:
		m_counter++;
		if (m_counter < m_params.releaseTimeSamps) {
			auto relGain = 20 * log10(1.f - (static_cast<double>(1.f * m_counter / m_params.releaseTimeSamps)));
			m_gain.setGaindB(m_params.sustainLeveldB + relGain);
		}
		else if (m_counter > m_params.releaseTimeSamps) {
			m_gain.setGaindB(-200);
		}

	break;
	}

	output = m_gain.apply(output);

	return output;

}
