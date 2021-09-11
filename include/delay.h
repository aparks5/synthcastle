#ifndef DELAY_H_
#define DELAY_H_

#include <vector>

class Delay
{
public:
	Delay(float sampleRate, float maxDelaySeconds);
	void update(float delayMs);
	void reset();
	float operator()(float val);

private:
	float m_fs;
	float m_curDelaySamps;
	float m_maxDelaySamps;
	std::vector<float> m_circBuff;
	size_t m_readIdx = 0;
	size_t m_writeIdx = 0;

};

#endif // DELAY_H_