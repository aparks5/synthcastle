/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SQUARE_H_ 
#define SQUARE_H_

#include "portaudio.h"

class Square
{
public:
    Square();
    virtual ~Square() {};
    void setSampleFreq(float frequency) { m_targetFreq = frequency; }
    void update();
    float generate();

private:
	float m_targetFreq;
    float m_output;
    float m_increment;

};

#endif // SQUARE_H_

