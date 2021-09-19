#ifndef DELAY_H_
#define DELAY_H_

#include <vector>

class Delay
{
public:
	Delay(float sampleRate, float maxDelaySeconds);
	void update(float delayMs, float feedbackRatio);
	void reset();
	float operator()(float val);
	void enableLowpass() { m_bLowpass = true; }
	void disableLowpass() { m_bLowpass = false; }

private:
	float m_fs;
	float m_feedback;
	float m_delayMs;
	size_t m_delaySamps;
	float m_maxDelaySamps;
	std::vector<float> m_circBuff;
	size_t m_writeIdx;
	size_t m_readIdx;
	size_t m_bufSize;
	// enable for lowpass feedback comb filter (used in freeverb/schroederverb)
	bool m_bLowpass;
	float m_lowpassDelayElement;

};

#endif // DELAY_H_