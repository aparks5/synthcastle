/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SAW_H_ 
#define SAW_H_

#include "oscillator.h"

class Saw : public Oscillator<float>
{
public:
    Saw(float fs);
    void freq(float frequency) override;
    float operator()() override;

};

#endif // SAW_H_

