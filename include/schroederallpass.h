#ifndef SCHROEDER_ALLPASS_H_
#define SCHROEDER_ALLPASS_H_

#include "delay.h"
#include "schroederallpass.h"

class SchroederAllpass
{
public:
	SchroederAllpass(float delayTimeMs, float feedbackCoef);
	float operator()(float in);


private:
	Delay m_delay;
	float m_delayTimeMs;
	float m_feedbackCoef;
	
};
#endif // SCHROEDER_ALLPASS_H_