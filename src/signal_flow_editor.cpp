#include "signal_flow_editor.h"

#include "gain.h"
#include "oscillator.h"
#include "output.h"
#include "graph.h"
#include <stack>
#include <SDL_scancode.h>

#include <memory>

// props to imnode color_node_editor.cpp for this genius algorithm
float evaluate(const Graph<Node>& graph, const int root_node)
{
	std::stack<int> postorder;
	dfs_traverse(graph, root_node, [&postorder](const int node_id) -> void { postorder.push(node_id); });

	std::stack<float> value_stack;
	while (!postorder.empty())
	{
		const int id = postorder.top();
		postorder.pop();
		const Node node = graph.node(id);

		switch (node.type)
		{
		case NodeType::GAIN:
		{
			const float input = value_stack.top();
			value_stack.pop();
			const float gain = value_stack.top();
			value_stack.pop();
			value_stack.push(input * gain);
		}
		break;
		case NodeType::VALUE:
		{
			// If the edge does not have an edge connecting to another node, then just use the value
			// at this node. It means the node's input pin has not been connected to anything and
			// the value comes from the node's UI.
			if (graph.num_edges_from_node(id) == 0ull)
			{
				value_stack.push(node.value);
			}
		}
		break;
		default:
			break;
		}
	}

	// The final output node isn't evaluated in the loop -- instead we just pop
	// the three values which should be in the stack.
	auto val = value_stack.top();
	value_stack.pop(); // stack should be empty now
	return val;
}

SignalFlowEditor::SignalFlowEditor()
	: m_pContext(nullptr)
	, m_graph()
	, m_nodes()
	, m_links()
	, m_currentId(0)
	, m_rootNodeId(-1)
{
    m_pContext = ImNodes::EditorContextCreate();
    ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

    ImNodesIO& io = ImNodes::GetIO();
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
    io.MultipleSelectModifier.Modifier = &ImGui::GetIO().KeyCtrl;

    ImNodesStyle& style = ImNodes::GetStyle();
    style.Flags |= ImNodesStyleFlags_GridLinesPrimary | ImNodesStyleFlags_GridSnapping;
}

SignalFlowEditor::~SignalFlowEditor()
{
	ImNodes::EditorContextFree(m_pContext);
}
	
void SignalFlowEditor::show()
{
	ImNodes::EditorContextSet(m_pContext);

    ImGui::Begin("signal_flow_editor");
    ImGui::TextUnformatted("A -- add node");
    ImGui::TextUnformatted("G -- add gain node");
    ImGui::TextUnformatted("O -- add output node");

    ImNodes::BeginNodeEditor();

    int node_id = 0;

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImNodes::IsEditorHovered()) {
        if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_A)) {
            node_id = ++m_currentId;
                ImNodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
                ImNodes::SnapNodeToGrid(node_id);
				auto newOsc = std::make_shared<Oscillator>(node_id);
				m_nodes.push_back(newOsc);
        }
		else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_G)) {
			node_id = ++m_currentId;
			ImNodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
			ImNodes::SnapNodeToGrid(node_id);
			auto newGain = std::make_shared<Gain>(node_id);
			m_nodes.push_back(newGain);
		}
		else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_O)) {
			node_id = ++m_currentId;
			ImNodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
			ImNodes::SnapNodeToGrid(node_id);
			auto newOutput = std::make_shared<Output>(node_id);
			m_nodes.push_back(newOutput);
		}

    }

    for (auto node : m_nodes) {
		// with overriding, no stupid case-switch needed
		node->display();
    }

    for (const Link& link : m_links) {
        ImNodes::Link(link.id, link.start_attr, link.end_attr);
    }

    ImNodes::EndNodeEditor();

    {
        Link link;
        if (ImNodes::IsLinkCreated(&link.start_attr, &link.end_attr))
        {
			link.id = ++m_currentId;
            m_links.push_back(link);
        }
    }

    {
        int link_id;
        if (ImNodes::IsLinkDestroyed(&link_id))
        {
            auto iter = std::find_if(
                m_links.begin(), m_links.end(), [link_id](const Link& link) -> bool {
                    return link.id == link_id;
                });
            assert(iter != m_links.end());
            m_links.erase(iter);
        }
    }

    ImGui::End();
}

