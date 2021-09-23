#include "allpass.h"
#include "constants.h"


Allpass::Allpass(float delayTimeMs, float feedbackRatio)
	: m_delay(SAMPLE_RATE, 2.0f)
	, m_delayTimeMs(delayTimeMs)
	, m_feedbackRatio(feedbackRatio)
{
	m_delay.update(m_delayTimeMs, feedbackRatio);
}

void Allpass::reset()
{
	m_delay.reset();
}

float Allpass::tap(float ms)
{
	return m_delay.tap(ms);
}

void Allpass::update(float delayTimeMs)
{
	m_delay.update(delayTimeMs, m_feedbackRatio);
}

float Allpass::operator()(float in)
{
	float out = m_delay();
	m_delay.write(in);
	// feed-forward path
	out -= (in * m_feedbackRatio);
	return out;
}
