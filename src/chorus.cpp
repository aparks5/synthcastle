#include "chorus.h"

Chorus::Chorus(float sampleRate)
	: delay(sampleRate, 0.1f)
	, lfo(sampleRate)
	, m_depth(1.f)
{
	lfo.freq(5.f);
	delay.update(35);
}

void Chorus::reset()
{
	delay.reset();
}

void Chorus::update()
{
	auto mod = m_depth * (0.5f * (1 + lfo()));
	delay.update(mod * 35);
}

float Chorus::operator()(float in)
{
	lfo();
	return delay(in);
}

