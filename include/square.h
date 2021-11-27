/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SQUARE_H_ 
#define SQUARE_H_

#include "oscillator.h"

class Square : public Oscillator<float>
{
public:
    Square(float fs);
    void freq(float frequency) override;
    float operator()() override;
    float operator()(float in) override {
        return 0.f;
    }

};

#endif // SQUARE_H_

