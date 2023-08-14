#include "quadmixer.h"
#include "imnodes.h"

QuadMixer::QuadMixer()
	: Node(NodeType::QUAD_MIXER, 0.f, NUM_PARAMS)
{}

float QuadMixer::process()
{
	return ((params[INPUT_A] +
		params[INPUT_B] +
		params[INPUT_C] +
		params[INPUT_D]) / 4.f
		);
}

void QuadMixer::display()
{
	ImNodes::BeginNode(params[NODE_ID]);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("QuadMix");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(params[INPUT_A_ID]);
	ImGui::TextUnformatted("A");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params[INPUT_B_ID]);
	ImGui::TextUnformatted("B");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params[INPUT_C_ID]);
	ImGui::TextUnformatted("C");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params[INPUT_D_ID]);
	ImGui::TextUnformatted("D");
	ImNodes::EndInputAttribute();

	ImNodes::BeginOutputAttribute(params[NODE_ID]);
	ImGui::TextUnformatted("Mix");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();
}
