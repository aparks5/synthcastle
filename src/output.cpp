#include "output.h"
#include "imnodes.h"

Output::Output(int id)
    : Node(NodeType::OUTPUT, id)
{
    params.push_back(0);
}

float Output::process()
{
    return 0;
}

void Output::display()
{
	ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Output");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(id << 8);
	ImGui::TextUnformatted("Sound");
	ImNodes::EndInputAttribute();

	ImNodes::EndNode();

}
