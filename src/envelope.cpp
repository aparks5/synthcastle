#include "envelope.h"
#include "math.h"
#include "constants.h"

Envelope::Envelope()
	: m_stage(EnvelopeStage::OFF)
	, m_gain(0)
	, m_bNoteOn(false)
{
}

void Envelope::setParams(EnvelopeParams params)
{
	m_params = params;
}

void Envelope::reset()
{
	m_stage = EnvelopeStage::ATTACK;
}

// TODO: envelope should just return a value, and then use that to convert to dB or apply to filter cutoff etc.
// as it is written it is only an amplitude envelope
// it should not be coupled to Gain in its class definition
float Envelope::apply(size_t numSamples)
{

	switch (m_stage) {

	case EnvelopeStage::ATTACK:
		if (m_params.attackTimeSamps != 0) {
			m_gain = m_gain + static_cast<double>(1.f / m_params.attackTimeSamps);
		}
		else {
			m_gain = 1.0f;
		}

		if (m_gain >= 1) {
			m_gain = 1;
			m_stage = EnvelopeStage::DECAY;
		}
	
		break;
	case EnvelopeStage::DECAY:
		if (m_params.decayTimeSamps > 0) {
			m_gain -= static_cast<float>((1.f /  m_params.decayTimeSamps) * pow(10, 1.f * m_params.sustainLeveldB / 20));
			if (m_gain <= 0) {
				m_gain = 0;
				m_stage = EnvelopeStage::OFF;
			}
		}

		if (m_gain == pow(10, 1.f * m_params.sustainLeveldB / 20)) {
			m_stage = EnvelopeStage::SUSTAIN;
		}

		break;
	case EnvelopeStage::SUSTAIN:
		m_gain = pow(10, 1.f * m_params.sustainLeveldB / 20);
		break;
	case EnvelopeStage::RELEASE:
		if (m_params.releaseTimeSamps > 0) {
			m_gain -= static_cast<double>(1.f / m_params.releaseTimeSamps);
			m_gain = (m_gain <= 0) ? 0 : m_gain;
		}
		else {
			m_stage = EnvelopeStage::OFF;
		}

		if (m_gain <= 0) {
			m_stage = EnvelopeStage::OFF;
		}

		break;
	case EnvelopeStage::OFF:

		m_gain -= static_cast<double>(1.f / (0.001f * SAMPLE_RATE));
		if (m_gain <= 0.f) {
			m_gain = 0.f;
		}


		break;
	}
	return m_gain;
}
