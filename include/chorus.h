#ifndef CHORUS_H_
#define CHORUS_H_

#include "delay.h"
#include "sine.h"

class Chorus : public Module
{
public:

	Chorus(float sampleRate, float depth, float feedbackRatio);
	void rate(float freq) { lfo.update(freq, 0, m_depth); }
	void depth(float gain) { m_depth = gain; }
	void reset();
	void update();
	float operator()(float in) override;
	float operator()() override { return 0.f; }

private:
	Delay delay;
	Sine lfo;
	float m_depth;
	float m_feedbackRatio;
};

#endif 