#ifndef PITCHSHIFT_H_
#define PITCHSHIFT_H_

class PitchShift : public Module
{
public:
	PitchShift(float sampleRate);
	void mix();
	void tune(int semitones);
	float operator()(float in);

private:
	float m_mix;
	// read faster or slower through the delay to alter pitch, crossfade where reads and writes overlap
	Delay m_delay;

};


#endif