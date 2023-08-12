/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef TRIANGLE_H_ 
#define TRIANGLE_H_ 

#include "oscillator.h"

class Triangle : public Oscillator
{
public:
    Triangle();
    Triangle(int sampleRate);
    float process() override;
    float process(float in) override { return 0.f; }
    void update() override;

private:
    bool m_bRising;
};

#endif // TRIANGLE_H_ 

