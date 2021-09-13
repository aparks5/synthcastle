#ifndef CHORUS_H_
#define CHORUS_H_

#include "delay.h"
#include "sine.h"

class Chorus
{
public:

	Chorus(float sampleRate);
	void rate(float freq) { lfo.freq(freq); }
	void depth(float gain) { m_depth = gain; }
	void reset();
	void update();
	float operator()(float in);

private:
	Delay delay;
	Sine lfo;
	float m_depth;
};

#endif 