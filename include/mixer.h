#ifndef MIX_H_
#define MIX_H_

#include "module.h"
#include "gain.h"
#include <string>
#include <vector>

class Mixer : public Module
{
public:
	Mixer(size_t fs);
	void addInput(Module* mod);
	std::vector<std::string> getInputsAsString();
	void setInputGain(size_t trackNum, float gaindB);

	float operator()() override;
	float operator()(float in) override { return 0.f; }
		
private:
	Gain m_overallGain;
	std::vector<Module*> m_inputs;
	std::vector<Gain> m_inputGains;
	float m_scaleFactor;
};

#endif