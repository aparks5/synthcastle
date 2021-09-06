#include "gain.h"
#include "math.h"

Gain::Gain()
	: m_fgain(1)
{
}

void Gain::setGaindB(float gaindB)
{
	m_fgain = dBtoFloat(gaindB);
}

void Gain::setGainf(float fgain)
{
	m_fgain = fgain;
}

int Gain::floatTodB(float f) const
{
	return 20 * log10(f);
}

float Gain::dBtoFloat(int db) const
{
	return pow(10, db / 20.f);
}

float Gain::apply(float samp)
{
	return samp * m_fgain;
}
