#include "oscillator.h"
#include "imnodes.h"

Oscillator::Oscillator(int id)
    : Node(NodeType::OSCILLATOR, id) 
{
    for (size_t idx = 0; idx < OscillatorParams::NUM_PARAMS; idx++) {
        params.push_back(0);
    }
}

float Oscillator::process()
{
    return 0;
}

void Oscillator::display()
{
	ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Oscillator");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(id << 8);
	ImGui::TextUnformatted("Mono Channel");
	ImNodes::EndInputAttribute();

	ImNodes::BeginStaticAttribute(id << 16);
	ImGui::PushItemWidth(120.0f);
	ImGui::SliderFloat("Frequency", &params[Oscillator::FREQ], 20., 10000.);
	ImGui::PopItemWidth();
	ImNodes::EndStaticAttribute();

	static int selected = -1;

	ImNodes::BeginStaticAttribute(id << 20);
	// Simplified one-liner Combo() API, using values packed in a single constant string
	ImGui::PushItemWidth(120.0f);
	static int waveform = 0;
	ImGui::Combo("Waveform", &waveform, "Saw\0Sine\0Square\0");
	params[Oscillator::WAVEFORM] = waveform;
	ImNodes::EndStaticAttribute();

	ImNodes::BeginOutputAttribute(id << 24);
	const float text_width = ImGui::CalcTextSize("Left Output").x;
	ImGui::Indent(120.f + ImGui::CalcTextSize("Frequency").x - text_width);
	ImGui::TextUnformatted("Left Output");
	ImNodes::EndOutputAttribute();
	ImNodes::EndNode();
}