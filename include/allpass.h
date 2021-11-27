#ifndef SCHROEDER_ALLPASS_H_
#define SCHROEDER_ALLPASS_H_

#include "delay.h"
#include "allpass.h"
#include "module.h"

class Allpass : public Module
{
public:
	Allpass(size_t fs, float delayTimeMs, float feedbackRatio);
	float operator()(float in) override;
	float operator()() override { return 0.f; }
	void reset();
	void update(float delayTimeMs);
	float tap(float ms);

private:
	Delay m_delay;
	float m_delayTimeMs;
	float m_feedbackRatio;
	
};
#endif // SCHROEDER_ALLPASS_H_