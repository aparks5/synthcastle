#ifndef TAPEDELAY_H_
#define TAPEDELAY_H_

/// shout out to SuperCollider DubEcho for inspiration for the algorithm http://sccode.org/1-h/fork

#include "module.h"
#include "delay.h"
#include "onepolehighpass.h"
#include "onepolelowpass.h"
#include "sine.h"


class TapeDelay : public Module
{
public:
	TapeDelay(size_t sampleRate);
	float operator()() override;
	float operator()(float in) override;
	void update(float delayTimeMs);

private:
	Delay m_delay;
	float m_delayTimeMs;
	float m_prevDelayTimeMs;
	Sine m_lfo;
	OnePoleHighpass m_hp;
	OnePoleLowpass m_lp;
	float m_lfoZeroCrossing;
	float m_slewDelta;
	float m_slewTime;


};

#endif