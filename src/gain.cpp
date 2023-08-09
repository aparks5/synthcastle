#include "gain.h"

#include "imnodes.h"

Gain::Gain(int id)
    : Node(NodeType::GAIN, id)
{
    for (size_t idx = 0; idx < GainParams::NUM_PARAMS; idx++) {
        params.push_back(0);
    }
}

float Gain::process()
{
    return 0;
}

void Gain::display()
{
	ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Gain");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(id << 8);
	ImGui::TextUnformatted("Mono Channel");
	ImNodes::EndInputAttribute();

	ImNodes::BeginStaticAttribute(id << 16);
	ImGui::PushItemWidth(120.0f);
	ImGui::SliderFloat("Gain", &params[Gain::GAIN], 0., 1.);
	ImGui::PopItemWidth();
	ImNodes::EndStaticAttribute();

	ImNodes::BeginOutputAttribute(id << 24);
	const float text_width = ImGui::CalcTextSize("Left Output").x;
	ImGui::Indent(120.f + ImGui::CalcTextSize("Frequency").x - text_width);
	ImGui::TextUnformatted("Left Output");
	ImNodes::EndOutputAttribute();
	ImNodes::EndNode();
}
