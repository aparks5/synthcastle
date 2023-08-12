#include "output.h"
#include "imnodes.h"

Output::Output()
    : Node(NodeType::OUTPUT, 0., NUM_PARAMS)
{
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

	ImNodes::BeginInputAttribute(params[Output::INPUT_L_ID]);
	ImGui::TextUnformatted("Left Out");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(params[Output::INPUT_R_ID]);
	ImGui::TextUnformatted("Right Out");
	ImNodes::EndInputAttribute();

	ImNodes::EndNode();

}
