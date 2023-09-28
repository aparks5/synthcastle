#include "chorus.h"

Chorus::Chorus(float sampleRate, float depth, float feedbackRatio)
	: Module(sampleRate)
	, delay()
	, lfo(sampleRate)
	, m_depth(depth)
	, m_feedbackRatio(feedbackRatio)
{
	//lfo.params[Oscillator::FREQ] = rate;
	//delay.update(10, m_feedbackRatio);
}

void Chorus::reset()
{
	delay.reset();
}

void Chorus::update()
{
	float mod = m_depth * lfo.process();
	//delay.update((mod * 10.) + 15., m_feedbackRatio);
}


float Chorus::operator()(float in)
{
	update();
	delay.inputs[Delay::INPUT] = in;
	delay.process();
	float out = delay.outputs[Delay::OUTPUT];
	m_output = out;
	return out;
}

