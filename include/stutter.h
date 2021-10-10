#ifndef STUTTER_H_
#define STUTTER_H_

#include "delay.h"

class Stutter
{
public:
	Stutter(float sampleRate, size_t bpm);
	void fs(size_t sampleRate);
	void bpm(size_t bpm);
	void beat(float beats);
	void update();
	void reset();
	float operator()(float in);

private:
	Delay m_delay;
	float m_fs;
	size_t m_bpm;
	float m_beat;

};


#endif // STUTTER_H_