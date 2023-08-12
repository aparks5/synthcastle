#include "gain.h"

#include "imnodes.h"

Gain::Gain(int id)
    : Node(NodeType::GAIN, id)
{
    for (size_t idx = 0; idx < GainParams::NUM_PARAMS; idx++) {
        params.push_back(0);
    }

	params[Gain::GAINMOD] = 1.f;
}

Gain::Gain()
    : Node(NodeType::GAIN, -1)
{
    for (size_t idx = 0; idx < GainParams::NUM_PARAMS; idx++) {
        params.push_back(0);
    }
}

void Gain::link(const float* src, const float* dst)
{
	m_map[dst] = src;
}



void Gain::display()
{
	ImNodes::BeginNode(params[GainParams::NODE_ID]);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Gain");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(params[GainParams::INPUT_ID]);
	ImGui::TextUnformatted("In");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params[GainParams::GAINMOD_ID]);
	ImGui::TextUnformatted("Mod");
	ImNodes::EndInputAttribute();

	ImGui::PushItemWidth(120.0f);
	ImGui::SliderFloat("Gain", &params[GainParams::GAIN], 0., 1.);
	ImGui::PopItemWidth();

	ImNodes::BeginOutputAttribute(params[GainParams::NODE_ID]);
	const float text_width = ImGui::CalcTextSize("Out").x;
	ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();
	ImNodes::EndNode();
}

void Gain::setGaindB(float gaindB)
{
	m_gain = dBtoFloat(gaindB);
}

int Gain::floatTodB(float f) const
{
	return 20 * log10(f);
}

float Gain::dBtoFloat(int db) const
{
	return pow(10, db / 20.f);
}

float Gain::process(float in)
{
	return in * params[Gain::GAIN] * params[Gain::GAINMOD];
}
