#include "comb.h"
#include "constants.h"

/// lowpass-feedback comb filter
Comb::Comb(float delayMs, float feedbackRatio)
	: m_delay(SAMPLE_RATE, 2.f)
{
	m_delay.enableLowpass();
	m_delay.update(delayMs, feedbackRatio);
}

float Comb::operator()(float in)
{
	return m_delay(in);
}