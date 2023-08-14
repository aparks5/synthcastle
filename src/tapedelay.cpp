#include "tapedelay.h"
#include <time.h>

TapeDelay::TapeDelay(size_t sampleRate)
	: Module(sampleRate)
	, m_delay(sampleRate, 1.f)
	, m_delayTimeMs(250.)
	, m_prevDelayTimeMs(250.)
	, m_lfo(sampleRate)
	, m_hp(sampleRate)
	, m_lp(sampleRate)
	, m_lfoZeroCrossing(-1)
	, m_slewDelta(0.f)
	, m_slewTime(m_delayTimeMs)
{
	m_delay.update(250, 0.8);
	m_hp.update(1000);
	m_lp.update(0.7);
	m_lfo.update(0.1);
	srand(time(NULL));
}

void TapeDelay::update(float delayTimeMs)
{
	m_delayTimeMs = delayTimeMs;
	m_delay.update(m_delayTimeMs, 0.8);
}

float TapeDelay::operator()()
{
	return 0.f;
}

float TapeDelay::operator()(float in)
{
	float temp = 0.;

	// bandpass
	temp = m_hp(in);
	temp = m_lp(temp);
	// distort
	temp = tanh(temp);
	m_delay.write(temp);

	m_delay.update(m_delayTimeMs + (m_lfo.process() * 2.f), 0.8);
	

	return m_delay();
}

