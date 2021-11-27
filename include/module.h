#ifndef MODULE_H_
#define MODULE_H_

class Module {
public:
	Module(size_t sampleRate);
	virtual float operator()() = 0; // sound generator
	virtual float operator()(float in) = 0; // sound processor
	float getOutput() { return m_output; }

protected:
	size_t m_sampleRate;
	float m_output;


};

#endif