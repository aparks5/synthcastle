/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef TRIANGLE_H_ 
#define TRIANGLE_H_ 

#include "oscillator.h"

class Triangle : public Oscillator<float>
{
public:
    Triangle(float fs);
    void freq(float frequency) override;
    float operator()() override;

private:
    bool m_bRising;
};

#endif // TRIANGLE_H_ 

