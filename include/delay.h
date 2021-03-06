#ifndef DELAY_H_
#define DELAY_H_

#include "module.h"
#include <vector>

class Delay : public Module
{
public:
	Delay(float sampleRate, float maxDelaySeconds);
	void update(float delayMs, float feedbackRatio);
	void reset();
	void write(float val);
	float operator()() override;
	float operator()(float in) override { return 0.f; }
	float tap(float ms);
	void skip();

private:
	float m_delayMs;
	size_t m_delaySamps;
	float m_maxDelaySamps;
	std::vector<float> m_circBuff;
	size_t m_writeIdx;
	size_t m_readIdx;
	size_t m_bufSize;
	float m_feedbackRatio;
	float m_feedbackOut;

};

#endif // DELAY_H_