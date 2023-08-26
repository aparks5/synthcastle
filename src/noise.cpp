#include "noise.h" 
#include "constants.h"
#include "stdio.h"
#include "math.h"
#include <random>

Noise::Noise(int sampleRate)
	: WaveForm(sampleRate)
	, m_counter(0.)
	, m_gen(m_rd())
	, m_distr(0, 128)
{
}

float Noise::process()
{
	return m_distr(m_gen) / 128.f;
}

