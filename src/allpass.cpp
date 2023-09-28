#include "allpass.h"
#include "constants.h"


Allpass::Allpass(size_t fs, float delayTimeMs, float feedbackRatio)
	: Module(fs)
	, m_delay()
	, m_delayTimeMs(delayTimeMs)
	, m_feedbackRatio(feedbackRatio)
{
	//m_delay.update(m_delayTimeMs, feedbackRatio);
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
	//m_delay.update(delayTimeMs, m_feedbackRatio);
}

float Allpass::operator()(float in)
{
	m_delay.inputs[Delay::INPUT] = in;
	m_delay.process();
	float out = m_delay.outputs[Delay::OUTPUT];
	// feed-forward path
	out -= (in * m_feedbackRatio);
	m_output = out;
	return out;
}
