/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef SQUARE_H_ 
#define SQUARE_H_

#include "portaudio.h"

class Square
{
public:
    Square();
    void reset();
    virtual ~Square() {};
    void freq(float frequency) { m_freq = frequency; }
    void update();
    float generate();

private:
	float m_freq;
    float m_output;
    float m_increment;

};

#endif // SQUARE_H_

