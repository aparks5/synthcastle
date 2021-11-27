#include "chorus.h"

Chorus::Chorus(float sampleRate, float rate, float depth, float feedbackRatio)
	: Module(sampleRate)
	, delay(sampleRate, 1.f)
	, lfo(sampleRate)
	, m_depth(depth)
	, m_feedbackRatio(feedbackRatio)
{
	lfo.freq(rate);
	delay.update(10, m_feedbackRatio);
}

void Chorus::reset()
{
	delay.reset();
}

void Chorus::update()
{
	float mod = m_depth * lfo();
	delay.update((mod * 10.) + 15., m_feedbackRatio);
}


float Chorus::operator()(float in)
{
	update();
	float out = delay();
	delay.write(in);
	return out;
}

