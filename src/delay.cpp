#include "delay.h"
#include "util.h"

#include <vector>

Delay::Delay(float sampleRate, float maxDelaySeconds)
	: m_fs(sampleRate)
	, m_delayMs(0)
	, m_delaySamps(0)
	, m_maxDelaySamps(maxDelaySeconds*sampleRate)
	, m_writeIdx(0)
	, m_readIdx(0)
{
	// allocate circular buffer
	m_circBuff.resize(m_maxDelaySamps);
	std::fill(m_circBuff.begin(), m_circBuff.end(), 0.f);
}

void Delay::update(float delayMs)
{
	m_delayMs = delayMs;

}

void Delay::reset()
{
	std::fill(m_circBuff.begin(), m_circBuff.end(), 0.f);
}

double Delay::operator()(float val)
{

	m_circBuff[m_writeIdx] = val;
	m_writeIdx++;
	if (m_writeIdx >= (m_circBuff.capacity() - 1)) {
		m_writeIdx = 0;
	}

	float integerDelay = 0.f;
	float fractionalDelay = modff(m_delayMs/1000.f*m_fs, &integerDelay);

	if ((m_writeIdx - abs(integerDelay)) < 0) {
		m_readIdx = (m_circBuff.capacity() - 1) + (m_writeIdx - integerDelay);
	}
	else {
		m_readIdx = m_writeIdx - static_cast<int>(integerDelay);
	}


	auto yn = 0.f;
	if ((m_readIdx == m_writeIdx) && integerDelay == 0.) {
		yn = val;
	}
	else {
		yn = m_circBuff[m_readIdx];
	}

	int prevReadIdx = m_readIdx - 1;
	if (prevReadIdx <= 0) {
		prevReadIdx = m_circBuff.capacity() - 1 - 1; // buffsize-1
	}
	
    double out = linearInterpolate(m_circBuff[prevReadIdx], yn, fractionalDelay);

	return out;


}
