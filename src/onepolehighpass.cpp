#include "onepolehighpass.h"
#include "math.h"
#include "constants.h"

OnePoleHighpass::OnePoleHighpass(size_t sampleRate)
	: Module(sampleRate)
	, m_cutoffFreqHz(100)
	, m_yn1(0.)
	, m_xn1(0.)
	, m_a0(0.)
	, m_a1(0.)
	, m_b0(0.)
{
	update(m_cutoffFreqHz);
}

void OnePoleHighpass::update(float cutoffFreqHz)
{
	m_cutoffFreqHz = cutoffFreqHz;
	m_yn1 = 0.;
	m_xn1 = 0.;
	// https://www.dspguide.com/ch19/2.htm
	m_b0 = exp(-2. * M_PI * m_cutoffFreqHz / m_sampleRate);
	m_a0 = (1 + m_b0) / 2;
	m_a1 = -1* (1 + m_b0) / 2;
}

float OnePoleHighpass::operator()()
{
	return 0.;
}

float OnePoleHighpass::operator()(float in)
{
	float yn = (m_a0 * in) + (m_a1 * m_xn1) + (m_b0 * m_yn1);
	m_xn1 = in;
	m_yn1 = yn;
	return yn;
}