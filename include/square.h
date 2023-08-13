/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SQUARE_H_ 
#define SQUARE_H_

#include "waveform.h"

class Square : public WaveForm 
{
public:
    Square(int sampleRate);
    float process() override;
	void update(float freq, float modfreq, float moddepth) override;
};

#endif // SQUARE_H_

