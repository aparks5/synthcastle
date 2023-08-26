#include "noise.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"
#include <random>

Noise::Noise(int sampleRate)
	: WaveForm(sampleRate)
	, m_gen(m_rd())
	, m_distr(0, 128)
	, m_bQuiet(false)
{
}

void Noise::update(float freq)
{
	m_bQuiet = (freq == 0.f);
}

float Noise::process()
{

	return m_bQuiet ? 0 : m_distr(m_gen) / 128.f;
}

