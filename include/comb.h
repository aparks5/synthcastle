#ifndef COMB_H_
#define COMB_H_

#include "delay.h"

class Comb
{
public:
	Comb(float delayMs, float feedbackRatio);
	float operator()(float in);

private:
	Delay m_delay;
	float m_feedback;
};

#endif // COMB_H_