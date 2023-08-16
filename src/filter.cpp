#include "filter.h"

#include "imnodes.h"

Filter::Filter()
	: Node(NodeType::FILTER, 0.f, NUM_PARAMS)
	, m_moogFilter(44100)
{
}

float Filter::process(float in)
{
	m_moogFilter.q(params[FiltParams::Q]);
	auto freq = params[FiltParams::FREQ] +
		(params[FiltParams::FREQ]
			* params[FiltParams::FREQMOD]
			* params[FiltParams::MODDEPTH]);
	m_moogFilter.freq(freq);
	float out = in;
	m_moogFilter.apply(&out, 1);
	return out;
}

void Filter::display()
{
	ImNodes::BeginNode(params[FiltParams::NODE_ID]);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Lowpass Filter");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(params[FiltParams::INPUT_ID]);
	ImGui::TextUnformatted("In");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params[FiltParams::FREQMOD_ID]);
	ImGui::TextUnformatted("Mod");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params[FiltParams::MODDEPTH_ID]);
	ImGui::TextUnformatted("Depth");
	ImNodes::EndInputAttribute();

	ImGui::PushItemWidth(120.0f);
	ImGui::DragFloat("Freq", &params[FiltParams::FREQ], 0., 5000.);
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(120.0f);
	ImGui::DragFloat("Q", &params[FiltParams::Q], 0., 10.);
	ImGui::PopItemWidth();

	ImNodes::BeginOutputAttribute(params[FiltParams::NODE_ID]);
	const float text_width = ImGui::CalcTextSize("Out").x;
	ImGui::Indent(120.f + ImGui::CalcTextSize("Out").x - text_width);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();
}


