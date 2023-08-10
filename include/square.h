/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SQUARE_H_ 
#define SQUARE_H_

#include "oscillator.h"

class Square : public Oscillator
{
public:
    Square(int id);
    float process() override;
    void update() override;


};

#endif // SQUARE_H_

