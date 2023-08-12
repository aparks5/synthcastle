#include "output.h"
#include "imnodes.h"

Output::Output()
    : Node(NodeType::OUTPUT)
{
	for (size_t idx = 0; idx < Output::NUM_PARAMS; idx++) {
		params.push_back(0);
	}
}

float Output::process()
{
    return value;
}

void Output::display()
{
	ImNodes::BeginNode(params[Output::NODE_ID]);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Output");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(params[Output::INPUT_ID]);
	ImGui::TextUnformatted("In");
	ImGui::SameLine();
	ImGui::PushItemWidth(120);
	ImGui::SliderFloat("##hidelabel", &value, -1.f, 1.f);
	ImGui::PopItemWidth();
	ImNodes::EndInputAttribute();

	ImNodes::EndNode();

}
