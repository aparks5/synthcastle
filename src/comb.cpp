#include "comb.h"
#include "constants.h"

/// lowpass-feedback comb filter
Comb::Comb(float delayMs, float feedbackRatio)
	: m_delay(SAMPLE_RATE, 2.f)
	, m_feedbackRatio(feedbackRatio)
	, m_damping(0.2f)
	, m_damping2(1.f-0.2f)
	, m_lowpassDelayElement(0.f)
{
	m_delay.update(delayMs, 0.f);
}

float Comb::operator()(float in)
{
	float out = m_delay();
	m_lowpassDelayElement = (out * m_damping2) + (m_damping * m_lowpassDelayElement);
	m_delay.write(in + (m_feedbackRatio * m_lowpassDelayElement));

	return out;
}