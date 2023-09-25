#include "oscillator.h"
#include "util.h"
#include "imnodes.h"

Oscillator::Oscillator()
    : Node(NodeType::OSCILLATOR, 0., NUM_PARAMS) 
	, m_sampleRate(44100)
{
	// todo: assert waveform enum and order of adding is correct
	{
		m_waveforms.push_back(std::make_shared<Saw>(m_sampleRate));
		m_waveforms.push_back(std::make_shared<Sine>(m_sampleRate));
		m_waveforms.push_back(std::make_shared<Square>(m_sampleRate));
		m_waveforms.push_back(std::make_shared<Triangle>(m_sampleRate));
		m_waveforms.push_back(std::make_shared<SampleAndHold>(m_sampleRate));
		m_waveforms.push_back(std::make_shared<Noise>(m_sampleRate));
	}
}

int Oscillator::lookupParam(std::string str)
{
	return m_lookup[str];
}

void Oscillator::update()
{
	auto modfreq = params[Oscillator::MODFREQ];
	auto moddepth = params[Oscillator::MODDEPTH];
	auto freq = params[Oscillator::FREQ]; // scale 0 to 1 to 0 to 22050
	auto coarse = params[Oscillator::TUNING_COARSE];
	auto fine = params[Oscillator::TUNING_FINE];

	freq = freq * semitoneToRatio(coarse) * semitoneToRatio(fine);

	if (modfreq != 0) {
		freq = freq + ((freq * moddepth) * modfreq);
	}
	
	// push mod freq, depth, tuning to freq calculations here, they are waveform-agnostic
	for (auto& wave : m_waveforms) {
		wave->update(freq);
	}
}

float Oscillator::process()
{
	return m_waveforms[(size_t)(params[OscillatorParams::WAVEFORM])]->process();
}

