#include "tapedelay.h"
#include <time.h>

TapeDelay::TapeDelay(size_t sampleRate)
	: Module(sampleRate)
	, m_delay(sampleRate, 1.f)
	, m_delayTimeMs(250.)
	, m_lfo(sampleRate)
	, m_hp(sampleRate)
	, m_lp(sampleRate)
{
	m_delay.update(250, 0.8);
	m_hp.update(1000);
	m_lp.update(0.7);
	m_lfo.freq(12);
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

	// modulate delay line
	srand(time(NULL));

	// only update the delay time once a cycle
	if (m_lfo() == 1.f) {
		m_delay.update((3 * (0.01 * (rand() % 100))) + m_delayTimeMs, 0.8);
	}
	return m_delay();
}

