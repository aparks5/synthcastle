#include "gain.h"
#include "math.h"

Gain::Gain()
	: m_fgain(1)
{
}

void Gain::setGaindB(int gaindB)
{
	m_fgain = dBtoFloat(gaindB);
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
