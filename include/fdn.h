#ifndef FDN_H_
#define FDN_H_

#include "delay.h"
#include <vector>

class FeedbackDelayNetwork 
{
public:
	FeedbackDelayNetwork(size_t order);
	float operator()(float in);
private:
	std::vector<Delay> m_delays;
	float m_primeTimes[8] = { 7,13,23,37,43,59,67,79 };
	std::vector<float> m_delayOutputs;
	float m_hadamard[4][4] = { { 1,1,1,1 }, {1,-1,1,-1}, {1,1,-1,-1}, {1,-1,-1,1} };
	float m_scaleFactor = 0.5f; // 1 / sqrt(order)
	float m_lowpassDelayElements[4];


};

#endif // FDN_H_
