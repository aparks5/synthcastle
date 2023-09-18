#include "pan.h"

#include "constants.h"
#include <math.h>


Pan::Pan(size_t fs)
	: Module(fs)
	, m_panLFO(fs)
	, m_panLFOdepth(1.f)
	, m_bEnableLFO(false)
	, m_percent(0.5)
	, m_leftGain(0.5)
	, m_rightGain(0.5)

{
	m_panLFO.update(1.f);
}

float Pan::operator()(float in)
{
	return 0.0f;
}

void Pan::operator()(std::array<std::array<float, 256>, 2>& outputBuffer)
{
	for (size_t samp = 0; samp < outputBuffer[0].size(); samp++) {
		if (m_bEnableLFO) {
			update(m_panLFO.process() * m_panLFOdepth);
		}
		else {
			update(m_percent);
		}
		outputBuffer[0][samp] *= m_leftGain;
		outputBuffer[1][samp] *= m_rightGain;
	}
}

void Pan::update(float percent)
{
	// https://dsp.stackexchange.com/questions/21691/algorithm-to-pan-audio
	float temp = M_PI * percent / 4;
	m_leftGain = sqrt(2)/2 * (cos(temp) + sin(temp));
	m_rightGain = sqrt(2)/2 * (cos(temp) - sin(temp));

}


