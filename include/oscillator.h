#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

template <class T>
class Oscillator
{
public:
	Oscillator() {}
	Oscillator(T fs, T freq, T step, T out) : m_fs(fs), m_freq(freq), m_step(step), m_out(out) {}
	virtual void freq(T frequency) = 0;
	virtual T operator()() = 0;

protected:
	T m_fs;
	T m_freq;
	T m_step;
	T m_out;
};


#endif // OSCILLATOR_H_