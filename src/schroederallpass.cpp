#include "schroederallpass.h"
#include "constants.h"


SchroederAllpass::SchroederAllpass(float delayTimeMs, float feedbackRatio)
	: m_delay(SAMPLE_RATE, 2.0f)
	, m_delayTimeMs(delayTimeMs)
	, m_feedbackRatio(feedbackRatio)
{
	m_delay.update(m_delayTimeMs, feedbackRatio);
}

float SchroederAllpass::operator()(float in)
{
	float out = m_delay();
	m_delay.write(in);
	// feed-forward path
	out -= (in * m_feedbackRatio);
	return out;
}