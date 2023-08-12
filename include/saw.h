/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SAW_H_ 
#define SAW_H_

#include "oscillator.h"

class Saw : public Oscillator
{
public:
    Saw();
    Saw(int sampleRate);
    float process() override;
    float process(float in) override { return 0.f; }
    void update() override;

};

#endif // SAW_H_

