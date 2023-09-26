#include "oscillator.h"
#include "util.h"
#include "imnodes.h"

Oscillator::Oscillator()
    : Node(NodeType::OSCILLATOR, "oscillator", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
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

void Oscillator::update()
{
	auto modfreq = params[MODFREQ];
	auto moddepth = params[MODDEPTH];
	auto freq = params[FREQ]; // scale 0 to 1 to 0 to 22050
	auto coarse = params[TUNING_COARSE];
	auto fine = params[TUNING_FINE];

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
	float out = m_waveforms[(size_t)(params[WAVEFORM])]->process();
	params[OUTPUT] = out;
	return 0;
}

