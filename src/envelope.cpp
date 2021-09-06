#include "envelope.h"
#include "math.h"
#include "constants.h"

Envelope::Envelope()
	: m_stage(EnvelopeStage::ATTACK)
	, m_counter(0)
	, m_gain(0)
	, m_bNoteOn(false)
{
}

void Envelope::setParams(EnvelopeParams params)
{
	m_params = params;
	reset();
}

void Envelope::reset()
{
	m_stage = EnvelopeStage::ATTACK;
	m_counter = 0;
}

// TODO: envelope should just return a value, and then use that to convert to dB or apply to filter cutoff etc.
// as it is written it is only an amplitude envelope
// it should not be coupled to Gain in its class definition
float Envelope::apply(size_t numSamples)
{
	m_counter += numSamples;

	switch (m_stage) {
	case EnvelopeStage::ATTACK:
		if (m_counter > m_params.attackTimeSamps) {
			m_counter = 0;
			m_stage = EnvelopeStage::DECAY;
		}
		else if (m_params.attackTimeSamps != 0) {
			
			m_gain = static_cast<double>(1.f * m_counter / m_params.attackTimeSamps);
			
		}
		else {
			m_gain = 0.000;
		}
	break;
	case EnvelopeStage::DECAY:
		if (m_counter > m_params.decayTimeSamps) {
			m_counter = 0;
			m_stage = EnvelopeStage::SUSTAIN;
		}
		else if (m_params.decayTimeSamps > 0) {
			m_gain = static_cast<float>((1.f * m_counter / m_params.decayTimeSamps) * pow(10, 1.f * m_params.sustainLeveldB / 20));
		}
		else {
			m_gain = pow(10, 1.f * m_params.sustainLeveldB / 20);
		}
		break;
	case EnvelopeStage::SUSTAIN:
		if (!m_bNoteOn) {
			m_stage = EnvelopeStage::RELEASE;
			m_counter = 0;
		}
        m_gain = pow(10, 1.f * m_params.sustainLeveldB / 20);
		break;
	case EnvelopeStage::RELEASE:
		if (m_counter > m_params.releaseTimeSamps) {
			m_gain = 0.001;
		}
		else if (m_counter < m_params.releaseTimeSamps) {
			auto relGain = static_cast<double>(1.f * m_counter / m_params.releaseTimeSamps);
			m_gain = pow(10, (m_params.sustainLeveldB * 1.f / 20)) - relGain;
			m_gain = (m_gain < 0) ? 0 : m_gain;
		}
		else {
			m_gain = 0;
		}
		break;
	}

	return m_gain;
}
