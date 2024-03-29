#include "envelope.h"
#include "math.h"
#include "constants.h"

Envelope::Envelope()
	: Node(NodeType::PROCESSOR, "envelope", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_stage(EnvelopeStage::OFF)
	, m_gain(0)
	, m_bTriggered(false)
{
	paramMap = {
		{"node_id", NODE_ID},
		{"trigger_id", TRIG_ID},
		{"trigger", TRIG},
		{"attack_ms", ATTACK_MS},
		{"decay_ms", DECAY_MS},
		{"sustain_db", SUSTAIN_DB},
		{"release_ms", RELEASE_MS}
	};

	inputMap = {
		{"input", INPUT},
		{"trig", TRIG},
	};

	outputMap = {
		{"output", OUTPUT},
	};

	initIdStrings();
}

void Envelope::process() noexcept
{
	float in = inputs[INPUT];

	if ((inputs[TRIG] != 0) && (!m_bTriggered)) { 
		m_stage = EnvelopeStage::ATTACK;
		m_bTriggered = true;
	}

	if (inputs[TRIG] == 0 && (m_stage != EnvelopeStage::ATTACK)) {
		m_stage = EnvelopeStage::RELEASE;
		m_bTriggered = false;
	}

	EnvelopeParams env(
		params[ATTACK_MS],
		params[DECAY_MS],
		params[SUSTAIN_DB],
		params[RELEASE_MS]);

	switch (m_stage) {
	case EnvelopeStage::ATTACK:
	{
		if (env.attackTimeSamps != 0) {
			m_gain += static_cast<double>(1.f / env.attackTimeSamps);
		}
		else {
			m_gain = 1.0f;
		}

		if (m_gain >= 1) {
			m_gain = 1;
			m_stage = EnvelopeStage::DECAY;
		}
	}
	break;
	case EnvelopeStage::DECAY:
	{
		if (env.decayTimeSamps > 0) {
			m_gain -= static_cast<float>((1.f / env.decayTimeSamps) * fmin(pow(10, 1.f * env.sustainLeveldB / 20), 0.0009));
			if (m_gain <= 0.001) {
				m_gain = 0;
				m_stage = EnvelopeStage::RELEASE;
			}
		}

		if (m_gain <= (pow(10, 1.f * env.sustainLeveldB / 20))) {
			m_gain = 0;
			m_stage = EnvelopeStage::SUSTAIN;
		}

	}
	break;
	case EnvelopeStage::SUSTAIN:
	{
		if (pow(10, 1.f * env.sustainLeveldB / 20) <= 0.0009f) {
			m_gain = 0;
			m_stage = EnvelopeStage::RELEASE;
		}
		else {
			m_gain = pow(10, 1.f * env.sustainLeveldB / 20);
		}
	}
	break;
	case EnvelopeStage::RELEASE:
	{
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
	}
	break;
	case EnvelopeStage::OFF:
	{
		m_gain -= static_cast<double>(1.f / (0.001f * SAMPLE_RATE));
		if (m_gain <= 0.f) {
			m_gain = 0.f;
		}
	}
	break;
	}

	outputs[OUTPUT] = in * m_gain;
}
