#ifndef ONEPOLEHP_H_
#define ONEPOLEHP_H_

#include "module.h"

// shout out to the OG Julius O Smith https://ccrma.stanford.edu/~jos/fp/One_Pole.html

class OnePoleHighpass : public Module
{
public:
	OnePoleHighpass(size_t sampleRate);
	void update(float cutoffFreqHz);
	float operator()() override;
	float operator()(float in) override;
private:
	float m_cutoffFreqHz;

	float m_xn1;
	float m_yn1;

	float m_a0;
	float m_a1;
	float m_b0;

};

#endif