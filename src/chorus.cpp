#include "chorus.h"

Chorus::Chorus(float sampleRate)
	: delay(sampleRate, 1.f)
	, lfo(sampleRate)
	, m_depth(.8f)
{
	lfo.freq(0.2f);
	delay.update(10, 0.2);
}

void Chorus::reset()
{
	delay.reset();
}

void Chorus::update()
{
	float mod = m_depth * lfo();
	delay.update((mod * 10.) + 15., 0);
}


float Chorus::operator()(float in)
{
	update();
	return delay(in);
}

