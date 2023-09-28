#include "comb.h"
#include "constants.h"

/// lowpass-feedback comb filter
Comb::Comb(size_t fs, float delayMs, float feedbackRatio)
	: Module(fs)
	, m_delay()
	, m_feedbackRatio(feedbackRatio)
	, m_damping(0.2f)
	, m_damping2(1.f-0.2f)
	, m_lowpassDelayElement(0.f)
{
	//m_delay.update(delayMs, 0.f);
}

float Comb::operator()(float in)
{
	m_delay.inputs[Delay::INPUT] = (in + (m_feedbackRatio * m_lowpassDelayElement));
	m_delay.process();
	float out = m_delay.outputs[Delay::OUTPUT];

	m_lowpassDelayElement = (out * m_damping2) + (m_damping * m_lowpassDelayElement);
	m_output = out;

	return out;
}