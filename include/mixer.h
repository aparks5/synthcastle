#ifndef MIX_H_
#define MIX_H_

#include "module.h"
#include "gain.h"
#include <string>
#include <unordered_map>
#include <vector>

class Mixer : public Module
{
public:
	Mixer(size_t fs);
	void addInput(Module* mod);
	std::vector<std::string> getInputsAsString();
	void setInputGain(std::string track, float gaindB);
	Module* getModuleByName(std::string track);
	void noteOn(size_t noteVal, std::string track);

	float operator()() override;
	float operator()(float in) override { return 0.f; }
		
private:
	Gain m_overallGain;
	std::unordered_map<std::string,Module*> m_inputs;
	std::unordered_map<std::string,float> m_inputGains;
	float m_scaleFactor;
};

#endif