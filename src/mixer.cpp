#include "mixer.h"

Mixer::Mixer(size_t fs)
	: Module(fs)
	, m_overallGain(fs)
	, m_scaleFactor(0.f)
{
}

void Mixer::addInput(Module* module)
{
	m_inputs.push_back(module);
	m_inputGains.push_back(Gain(m_sampleRate));
	m_scaleFactor = 1 / (m_inputs.size() * sqrt(2));
	m_overallGain.setGainf(m_scaleFactor);
}

std::vector<std::string> Mixer::getInputsAsString()
{
	std::vector<std::string> inputStrings;

	for (auto mod : m_inputs) {
		std::string name = mod->getName();
		if (name.size() > 0) {
			inputStrings.push_back(name);
		}
		else {
			inputStrings.push_back("unnamed module (!)");
		}
	}

	return inputStrings;
}

void Mixer::setInputGain(size_t trackNum, float gaindB)
{
	if (trackNum < m_inputGains.size()) {
		m_inputGains[trackNum].setGaindB(gaindB);
	}
}

float Mixer::operator()()
{
	float output = 0.f;
	for (size_t idx = 0; idx < m_inputs.size(); idx++) {
		float temp = (*m_inputs[idx])();
		temp = m_inputGains[idx](temp);
		temp = m_overallGain(temp);
		output += temp;
	}

	return output;

}