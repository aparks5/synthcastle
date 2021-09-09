/// Copyright(c) 2021. Anthony Parks. All rights reserved.
#ifndef TRIANGLE_H_ 
#define TRIANGLE_H_ 

#include "portaudio.h"

class Triangle
{
public:
    Triangle();
    void reset();
    virtual ~Triangle() {};
	void freq(float frequency) { m_freq = frequency; }
	void update();
	float generate();

private:
    float m_freq;
    float m_output;
    float m_incrementBase;
    float m_increment;

};

#endif // TRIANGLE_H_ 

