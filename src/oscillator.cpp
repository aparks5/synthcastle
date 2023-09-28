#include "oscillator.h"
#include "util.h"
#include "imnodes.h"

Oscillator::Oscillator()
    : Node(NodeType::PROCESSOR, "oscillator", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
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

	paramMap = {
		{"node_id", NODE_ID},
		{"freq_id", FREQ_ID},
		{"output_id", OUTPUT_ID},
		{"modfreq_id", MODFREQ_ID},
		{"moddepth_id", MODDEPTH_ID},
		{"freq", FREQ},
		{"tuning_fine", TUNING_FINE},
		{"tuning_coarse", TUNING_COARSE},
		{"waveform", WAVEFORM}
	};

	inputMap = {
		{"modfreq", MODFREQ},
		{"moddepth", MODDEPTH}
	};

	outputMap = {
		{"output", OUTPUT}
	};

	inputIdStrings = {
		"modfreq_id",
		"moddepth_id"
	};

	outputIdStrings = {
		"output_id",
	};

}

void Oscillator::update()
{
	auto modfreq = inputs[MODFREQ];
	auto moddepth = inputs[MODDEPTH];

	auto freq = params[FREQ]; // TODO: scale 0 to 1 to 0 to 22050
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

void Oscillator::process() noexcept
{
	float out = m_waveforms[(size_t)(params[WAVEFORM])]->process();
	outputs[OUTPUT] = out;
}

