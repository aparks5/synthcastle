/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SAW_H_ 
#define SAW_H_

#include "waveform.h"

class Saw : public WaveForm 
{
public:
    Saw(int sampleRate);
    float process() override;
	void update(float freq, float modfreq, float moddepth) override;
};

#endif // SAW_H_

