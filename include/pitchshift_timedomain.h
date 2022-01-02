#ifndef PITCHSHIFT_TIMEDOMAIN_H_
#define PITCHSHIFT_TIMEDOMAIN_H_

#include "delay.h"
#include "saw.h"
#include "envelope.h"
#include "sine.h"

// naive pitch shifting method using a delay line and speeding up reading
// a C++ port of the following PureData patch: http://msp.ucsd.edu/techniques/v0.11/book-html/node125.html
class PitchShift : public Module
{
public:
	PitchShift(size_t fs);
	float operator()(float in) override;
	float operator()() override { return 0.f; }
	void update(float numSemitones);

private:
	Delay m_delay;
	Delay m_delayInv;
	Saw m_saw;
	Sine m_env;
	size_t m_shiftSemitones;
	bool m_bShiftUp;
	float m_windowTime;
};

#endif