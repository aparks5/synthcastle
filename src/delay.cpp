#include "delay.h"
#include "util.h"

#include <vector>

Delay::Delay(float sampleRate, float maxDelaySeconds)
	: Module(sampleRate)
	, m_delayMs(0)
	, m_delaySamps(0)
	, m_maxDelaySamps(maxDelaySeconds*sampleRate)
	, m_writeIdx(0)
	, m_readIdx(0)
	, m_feedbackRatio(0.f)
	, m_feedbackOut(0.f)
{
	// allocate circular buffer
	m_circBuff.resize(static_cast<size_t>(m_maxDelaySamps));
	std::fill(m_circBuff.begin(), m_circBuff.end(), 0.f);
	m_bufSize = m_circBuff.capacity() - 1;
}

float Delay::tap(float ms)
{

	int integerDelay = (int)(ms / 1000.f * m_sampleRate);

	if (static_cast<int>(m_writeIdx) - integerDelay < 0) {
		return m_circBuff[m_bufSize + 1 + (m_writeIdx - integerDelay)];
	}
	else {
		return m_circBuff[m_writeIdx - static_cast<int>(integerDelay)];
	}

}

void Delay::update(float delayMs, float feedbackRatio)
{
	m_delayMs = delayMs;
	m_feedbackRatio = feedbackRatio;
}

void Delay::reset()
{
	std::fill(m_circBuff.begin(), m_circBuff.end(), 0.f);
}

void Delay::write(float val)
{
	m_circBuff[m_writeIdx] = val + (m_feedbackRatio * m_feedbackOut);

	m_writeIdx++;
	if (m_writeIdx > m_bufSize) {
		m_writeIdx = 0;
	}
}

float Delay::operator()()
{

	// find delay index and separate fractional delay for interpolation
	float fractionalDelay = (m_delayMs / 1000.f * m_sampleRate) - (int)(m_delayMs / 1000.f * m_sampleRate);
	int integerDelay = (int)(m_delayMs / 1000.f * m_sampleRate);

	if (static_cast<int>(m_writeIdx) - integerDelay < 0) {
		m_readIdx = m_bufSize + 1 + (m_writeIdx - integerDelay); 
	}
	else {
		m_readIdx = m_writeIdx - static_cast<int>(integerDelay);
	}

	if (static_cast<size_t>(m_delayMs / 1000.f * m_sampleRate) == 0) {
		return m_circBuff[m_writeIdx];
	}

	// for 0 delay, interpolate the input with the prev output
	auto yn = 0.f;
	if ((m_writeIdx == m_readIdx) && integerDelay < 1.) {
		yn = m_circBuff[m_writeIdx];
	}
	else {
		yn = m_circBuff[m_readIdx];
	}

	// find previous delay
	int prevReadIdx = static_cast<int>(m_readIdx) - 1;
	if (prevReadIdx < 0) {
		prevReadIdx = static_cast<int>(m_bufSize); 
	}

	auto yn1 = m_circBuff[prevReadIdx];

	float out = linearInterpolate(yn, yn1, fractionalDelay);
	m_feedbackOut = out;
	return out;
}
