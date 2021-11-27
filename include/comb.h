#ifndef COMB_H_
#define COMB_H_

#include "delay.h"

class Comb : public Module
{
public:
	Comb(size_t fs, float delayMs, float feedbackRatio);
	float operator()(float in);

private:
	Delay m_delay;
	float m_feedbackRatio;
	float m_damping;
	float m_damping2; // is just (1 - m_damping)
	float m_lowpassDelayElement;
};

#endif // COMB_H_