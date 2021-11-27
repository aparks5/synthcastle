#include "onepolelowpass.h"

OnePoleLowpass::OnePoleLowpass(size_t fs)
	: Module(fs)
	, m_damp1(0.2)
	, m_damp2(1-m_damp1)
	, m_delayElement(0.f)
{
}

void OnePoleLowpass::update(float damping)
{
	m_damp1 = damping;
	m_damp2 = 1.f - damping;
}

void OnePoleLowpass::reset()
{
	m_delayElement = 0.f;
}

float OnePoleLowpass::operator()(float in)
{
	m_delayElement = (in * m_damp2) + (m_damp1 * m_delayElement);
	return m_delayElement;
}