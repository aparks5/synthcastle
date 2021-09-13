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
	m_circBuff.resize(static_cast<size_t>(m_maxDelaySamps));
	std::fill(m_circBuff.begin(), m_circBuff.end(), 0.f);
	m_bufSize = m_circBuff.capacity() - 1;
}

void Delay::update(float delayMs)
{
	m_delayMs = delayMs;

}

void Delay::reset()
{
	std::fill(m_circBuff.begin(), m_circBuff.end(), 0.f);
}

float Delay::operator()(float val)
{

	m_circBuff[m_writeIdx] = val;

	// find delay index and separte fractional delay for interpolation
	float integerDelay = 0.f;
	float fractionalDelay = modff(m_delayMs/1000.f*m_fs, &integerDelay);

	if (m_writeIdx - integerDelay <= 0) {
		m_readIdx = m_bufSize + (m_writeIdx - (size_t)integerDelay);
	}
	else {
		m_readIdx = m_writeIdx - static_cast<int>(integerDelay);
	}

	if (m_delayMs / 1000 * m_fs == 0) {
		return val;
	}

	// for 0 delay, interpolate the input with the prev output
	auto yn = 0.f;
	if ((m_readIdx == m_writeIdx) && integerDelay < 1.) {
		yn = val;
	}
	else {
		yn = m_circBuff[m_readIdx];
	}

	// find previous delay
	int prevReadIdx = static_cast<int>(m_readIdx) - 1;
	if (prevReadIdx <= 0) {
		prevReadIdx = static_cast<int>(m_bufSize) - 1; 
	}

	auto yn1 = m_circBuff[prevReadIdx];

	// update write idx
	m_writeIdx++;
	if (m_writeIdx >= m_bufSize) {
		m_writeIdx = 0;
	}
	
	return linearInterpolate(yn, yn1, fractionalDelay);
}
