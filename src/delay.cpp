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
	m_delaySamps = static_cast<size_t>(delayMs / 1000.f * m_fs);

	if (m_writeIdx - m_delaySamps < 0) {
		m_readIdx = m_circBuff.capacity() - 1 + (m_writeIdx - m_delaySamps);
	}
}

void Delay::reset()
{
	std::fill(m_circBuff.begin(), m_circBuff.end(), 0.f);
}

float Delay::operator()(float val)
{
	m_writeIdx++;
	m_readIdx++;

	if (m_writeIdx >= (m_circBuff.capacity() - 1)) {
		m_writeIdx = 0;
	}

	if (m_readIdx >= (m_circBuff.capacity() - 1)) {
		m_readIdx = 0;
	}

	m_circBuff[m_writeIdx] = val;


	auto prevReadIdx = m_readIdx - 1;
	if (prevReadIdx < 0) {
		prevReadIdx = m_circBuff.capacity() - 1;
	}


	float fractionalDelay = 0.f;
	float integerDelay = modff(m_delayMs, &fractionalDelay);
	(void)integerDelay;
	return linearInterpolate(m_circBuff[m_readIdx], m_circBuff[prevReadIdx], fractionalDelay);
}
