#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

#include "module.h"

template <class T>
class Oscillator : public Module
{
public:
	Oscillator(size_t sampleRate) : Module(sampleRate) { };
	Oscillator(T fs, T freq, T step, T out) : Module(fs), m_freq(freq), m_step(step), m_out(out) {}
	virtual void freq(T frequency) = 0;
	float operator()() override { return 0.f; }
	float operator()(float in) override { return 0.f; }

protected:
	T m_freq;
	T m_step;
	T m_out;
};


#endif // OSCILLATOR_H_