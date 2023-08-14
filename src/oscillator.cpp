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
	}
}

void Oscillator::update()
{
	auto modfreq = params[Oscillator::MODFREQ];
	auto moddepth = params[Oscillator::MODDEPTH];
	auto freq = params[Oscillator::FREQ];
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

void Oscillator::display()
{
	ImNodes::BeginNode(params[OscillatorParams::NODE_ID]);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Oscillator");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(params[OscillatorParams::FREQ_ID]);
	ImGui::PushItemWidth(120.0f);
	ImGui::DragFloat("Frequency", &params[OscillatorParams::FREQ], 1.f, 0.00, 2000.);
	ImGui::PopItemWidth();
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params[OscillatorParams::MODFREQ_ID]);
	ImGui::TextUnformatted("FreqMod");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params[OscillatorParams::MODDEPTH_ID]);
	ImGui::TextUnformatted("FreqModDepth");
	ImNodes::EndInputAttribute();

	ImGui::PushItemWidth(120.0f);
	ImGui::DragFloat("Coarse Tuning", &params[OscillatorParams::TUNING_COARSE], 1.f, -36.00, 36.);
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(120.0f);
	ImGui::DragFloat("Fine Tuning", &params[OscillatorParams::TUNING_FINE], 1.f, 0.00, 1.);
	ImGui::PopItemWidth();

	int selected = -1;

	// Simplified one-liner Combo() API, using values packed in a single constant string
	ImGui::PushItemWidth(120.0f);
	int waveform = params[Oscillator::WAVEFORM];
	ImGui::Combo("Waveform", &waveform, "Saw\0Sine\0Square\0Triangle\0");
	params[Oscillator::WAVEFORM] = waveform;

	ImNodes::BeginOutputAttribute(params[OscillatorParams::NODE_ID]);
	const float text_width = ImGui::CalcTextSize("Out").x;
	ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();
}