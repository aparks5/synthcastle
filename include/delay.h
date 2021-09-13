#ifndef DELAY_H_
#define DELAY_H_

#include <vector>

class Delay
{
public:
	Delay(float sampleRate, float maxDelaySeconds);
	void update(float delayMs);
	void reset();
	double operator()(float val);

private:
	float m_fs;
	float m_delayMs;
	size_t m_delaySamps;
	float m_maxDelaySamps;
	std::vector<float> m_circBuff;
	size_t m_writeIdx;
	size_t m_readIdx;

};

#endif // DELAY_H_