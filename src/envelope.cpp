#include "envelope.h"
#include "math.h"
#include "constants.h"

Envelope::Envelope()
	: Node(NodeType::ENVELOPE, 0, NUM_PARAMS)
	, m_stage(EnvelopeStage::OFF)
	, m_gain(0)
	, m_bTriggered(false)
{
}

int Envelope::lookupParam(std::string str)
{
	return m_lookup[str];
}

// TODO: envelope should just return a value, and then use that to convert to dB or apply to filter cutoff etc.
// as it is written it is only an amplitude envelope
// it should not be coupled to Gain in its class definition
float Envelope::process(float in)
{
	if (params[TRIG] != 0) {
		m_stage = EnvelopeStage::ATTACK;
		params[TRIG] = 0;
		m_bTriggered = true;
	}
	else {
		if (m_bTriggered == false) {
			m_stage = EnvelopeStage::RELEASE;
		}
	}

	EnvelopeParams env(params[ATTACK_MS],
		params[DECAY_MS],
		params[SUSTAIN_DB],
		params[RELEASE_MS]);

	switch (m_stage) {

	case EnvelopeStage::ATTACK:
		if (env.attackTimeSamps != 0) {
			m_gain = m_gain + static_cast<double>(1.f / env.attackTimeSamps);
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
		if (env.decayTimeSamps > 0) {
			m_gain -= static_cast<float>((1.f /  env.decayTimeSamps) * pow(10, 1.f * env.sustainLeveldB / 20));
			if (m_gain <= 0) {
				m_gain = 0;
				m_stage = EnvelopeStage::OFF;
			}
		}

		if (m_gain == pow(10, 1.f * env.sustainLeveldB / 20)) {
			m_stage = EnvelopeStage::SUSTAIN;
		}

		break;
	case EnvelopeStage::SUSTAIN:
		m_stage = EnvelopeStage::RELEASE;
		m_gain = pow(10, 1.f * env.sustainLeveldB / 20);
		break;
	case EnvelopeStage::RELEASE:
		if (env.releaseTimeSamps > 0) {
			m_gain -= static_cast<double>(1.f / env.releaseTimeSamps);
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
		m_bTriggered = false;


		break;
	}
	return in * m_gain;
}
