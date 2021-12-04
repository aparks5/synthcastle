#ifndef MODULE_H_
#define MODULE_H_

#include <string>

class Module {
public:
	Module(size_t sampleRate);
	virtual float operator()() = 0; // sound generator
	virtual float operator()(float in) = 0; // sound processor
	float getOutput() const { return m_output; }
	std::string getName() const { return m_name; }
	void setName(std::string name) { m_name = name; }

protected:
	size_t m_sampleRate;
	float m_output;
	std::string m_name;


};

#endif