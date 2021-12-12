#include "mixer.h"

Mixer::Mixer(size_t fs)
	: Module(fs)
	, m_overallGain(fs)
	, m_scaleFactor(0.f)
{
}

void Mixer::addInput(Module* module)
{
	m_inputs.emplace(module->getName(), module);
	m_inputGains.emplace(module->getName(), 0.f);
	m_scaleFactor = 1 / (m_inputs.size() * sqrt(2));
	m_overallGain.setGainf(m_scaleFactor);
}

std::vector<std::string> Mixer::getInputsAsString()
{
	std::vector<std::string> inputStrings;

	for (const auto moduleKeyValue : m_inputs) {
		inputStrings.push_back(moduleKeyValue.first);
	}

	return inputStrings;
}

void Mixer::setInputGain(std::string track, float gaindB)
{
	m_inputGains[track] = gaindB;
}

Module* Mixer::getModuleByName(std::string track)
{
	return m_inputs[track];
}

void Mixer::noteOn(size_t noteVal, std::string track)
{
	Module* pMod = m_inputs[track];
	if (pMod) {
		pMod->noteOn(noteVal);
	}
}

void Mixer::noteOff(size_t noteVal, std::string track)
{
	Module* pMod = m_inputs[track];
	if (pMod) {
		pMod->noteOff(noteVal);
	}
}


float Mixer::operator()()
{
	float output = 0.f;
	for (const auto& inputKeyValue : m_inputs) {
		std::string moduleName = inputKeyValue.first;
		Module* pModule = inputKeyValue.second;
		
		float temp = 0.f;
		if (pModule) {
			temp = (*pModule)();
		}
		Gain trackGain(m_sampleRate);
		trackGain.setGaindB(m_inputGains[moduleName]);
		temp = trackGain(temp);
		temp = m_overallGain(temp);
		output += temp;

	}

	return output;

}