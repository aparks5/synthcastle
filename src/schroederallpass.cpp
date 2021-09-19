#include "schroederallpass.h"
#include "constants.h"


SchroederAllpass::SchroederAllpass(float delayTimeMs, float feedbackCoef)
	: m_delay(SAMPLE_RATE, 2.0f)
	, m_delayTimeMs(delayTimeMs)
	, m_feedbackCoef(feedbackCoef)
{
	m_delay.update(m_delayTimeMs, feedbackCoef);
}

float SchroederAllpass::operator()(float in)
{
	float out = 0.f;
	out = m_delay(in);
	out -= (in * m_feedbackCoef);
	return out;
}
