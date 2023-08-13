#include "oscillator.h"
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
	for (auto& wave : m_waveforms) {
		wave->update(params[OscillatorParams::FREQ],
			params[OscillatorParams::MODFREQ],
			params[OscillatorParams::MODDEPTH]);
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

	static int selected = -1;

	// Simplified one-liner Combo() API, using values packed in a single constant string
	ImGui::PushItemWidth(120.0f);
	static int waveform = 0;
	ImGui::Combo("Waveform", &waveform, "Saw\0Sine\0Square\0Triangle\0");
	params[Oscillator::WAVEFORM] = waveform;

	ImNodes::BeginOutputAttribute(params[OscillatorParams::NODE_ID]);
	const float text_width = ImGui::CalcTextSize("Out").x;
	ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();
}