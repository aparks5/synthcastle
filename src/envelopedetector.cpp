#include "envelopedetector.h"

EnvelopeDetector::EnvelopeDetector(float sampleRate)
	: Module(sampleRate)
	: m_params()
{

}

EnvelopeDetector::EnvelopeDetector(EnvelopeDetectorParams params)
	: Module(sampleRate)
	, m_env(0.f)
	, m_fAttack(0.001f)
	, m_fDecay(0.001f)
{
	update(params);
	reset();
}

void EnvelopeDetector::update(EnvelopeDetectorParams params)
{
	m_params.attackMs = clamp(0.001f, 500);
	m_params.decayMs = clamp(0.001f, 500);
	reset();
}

void EnvelopeDetector::reset()
{
	m_fAttack = exp(log(0.368) / (m_params.attackMs * m_fs * 0.001));
	m_fDecay = exp(log(0.368) / (m_params.decayMs * m_fs * 0.001));
	m_env = 0.f;
}

float EnvelopeDetector::operator()(float in)
{
	// rectify
	float input = fabs(in);

	// square
	input *= input;

	float squared = 0.f;

	// --- do the RC simulator detection with attack or release
	if (in > m_rms) {
		m_env = m_fAttack * (m_env - in) + in;
	}
	else {
		m_env = m_fRelease * (m_env - in) + in;
	}

	if (m_env <= 0) {
		return -96.f;
	}

	return 20.f * log10(sqrt(m_env));

}