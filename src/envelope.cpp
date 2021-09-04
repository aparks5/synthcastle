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
}

void Envelope::reset()
{
	m_counter = 0;
	m_stage = ATTACK;
}

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
		// TODO: once sustain level is accounted for, some variation of subtracting from 1
		// and proceeding to sustain level will be needed
		// m_gain.setGaindB(20 * log10(1 - m_counter / m_params.attackTimeSamps * 1.f));
		m_gain.setGaindB(0);
		if (m_counter > m_params.decayTimeSamps) {
			m_counter = 0;
			m_stage = SUSTAIN;
		}
		break;
	case SUSTAIN:
		m_counter++;
		m_gain.setGaindB(0);
		if (m_counter > (0.25*SAMPLE_RATE)) {
			m_counter = 0;
			m_stage = RELEASE;
		}
		break;
	case RELEASE:
		m_counter++;
		if (m_params.releaseTimeSamps) {
			auto relGain = 20 * log10(1.f - (static_cast<double>(1.f * m_counter / m_params.releaseTimeSamps)));
			m_gain.setGaindB(relGain);
		}
		if (m_counter > m_params.releaseTimeSamps) {
			m_counter = 0;
			m_gain.setGaindB(-60);
		}
		break;
	}

	output = m_gain.apply(output);

	return output;

}
