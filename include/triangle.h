/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef TRIANGLE_H_ 
#define TRIANGLE_H_ 

#include "waveform.h"

class Triangle : public WaveForm 
{
public:
    Triangle(int sampleRate);
    float process() override;
	void update(float freq, float modfreq, float moddepth) override;

private:
    bool m_bRising;
};

#endif // TRIANGLE_H_ 

