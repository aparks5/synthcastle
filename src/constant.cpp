#include "constant.h"

#include "imnodes.h"

Constant::Constant()
	: Node(NodeType::CONSTANT, 0.)
{
	for (size_t idx = 0; idx < NUM_PARAMS; idx++) {
		params.push_back(0);
	}
}

float Constant::process()
{
	return value;
}

void Constant::display()
{
	ImNodes::BeginNode(params[Constant::NODE_ID]);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Constant");
	ImNodes::EndNodeTitleBar();

	ImGui::PushItemWidth(120.0f);
	ImGui::DragFloat("Value", &value, 0.5f, 0, 1.);
	ImGui::PopItemWidth();

	ImNodes::BeginOutputAttribute(params[Constant::NODE_ID]);
	ImGui::TextUnformatted("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();

}
