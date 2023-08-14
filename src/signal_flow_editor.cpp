#include "signal_flow_editor.h"

#include "gain.h"
#include "quadmixer.h"
#include "midi.h"
#include "constant.h"
#include "oscillator.h"
#include "output.h"
#include "value.h"
#include "nodegraph.h"
#include <stack>
#include <SDL_scancode.h>
#include "sine.h"
#include <memory>

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

const NodeGraph* SignalFlowEditor::graph() const {
	// NodeGraph copy(m_graph); <-- too many heap allocations, eventually causes crash
	return &m_graph;
}

void SignalFlowEditor::show()
{
	ImNodes::EditorContextSet(m_pContext);

    ImGui::Begin("signal_flow_editor");
    ImGui::TextUnformatted("A -- add osc node");
    ImGui::TextUnformatted("C -- add const node");
    ImGui::TextUnformatted("G -- add gain node");
    ImGui::TextUnformatted("S -- add sound output node");
	ImGui::TextUnformatted("M -- add midi input node");
	ImGui::TextUnformatted("X -- add quad mixer node");

    ImNodes::BeginNodeEditor();

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImNodes::IsEditorHovered()) {
        if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_A)) {
				auto oscNode = std::make_shared<Oscillator>();
				auto oscMod = std::make_shared<Value>(INVALID_PARAM_VALUE);
				auto freq = std::make_shared<Value>(INVALID_PARAM_VALUE);
				auto depthMod = std::make_shared<Value>(0.f);
				auto depthModId = m_graph.insert_node(depthMod);
				auto oscModId = m_graph.insert_node(oscMod);
				auto freqId = m_graph.insert_node(freq);
				auto id = m_graph.insert_node(oscNode);
				oscNode->params[Oscillator::MODDEPTH_ID] = depthModId;
				oscNode->params[Oscillator::MODFREQ_ID] = oscModId;
				oscNode->params[Oscillator::FREQ_ID] = freqId;
				oscNode->params[Oscillator::NODE_ID] = id;
				m_graph.insert_edge(id, depthModId);
				m_graph.insert_edge(id, oscModId);
				m_graph.insert_edge(id, freqId);
				m_nodes.push_back(oscNode);
                const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
				ImNodes::SetNodeScreenSpacePos(id, click_pos);
        }
		else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_C)) {
			auto kNode = std::make_shared<Constant>();
			auto kId = m_graph.insert_node(kNode);
			kNode->params[Constant::NODE_ID] = kId;
			m_nodes.push_back(kNode);
			const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
			ImNodes::SetNodeScreenSpacePos(kId, click_pos);
		}
		else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_M)) {
			auto node = std::make_shared<MIDI>();
			auto id = m_graph.insert_node(node);
			node->params[MIDI::NODE_ID] = id;
			m_nodes.push_back(node);
			const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
			ImNodes::SetNodeScreenSpacePos(id, click_pos);
		}
		else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_G)) {
			auto gainNode = std::make_shared<Gain>();
			auto gainIn = std::make_shared<Value>(0.f);
			auto gainMod = std::make_shared<Value>(1.f);
			// you need to assign this gain id for linking
			auto gainModId = m_graph.insert_node(gainMod);
			auto gainInId = m_graph.insert_node(gainIn);
			auto gainId = m_graph.insert_node(gainNode);
			gainNode->params[Gain::NODE_ID] = gainId;
			gainNode->params[Gain::INPUT_ID] = gainInId;
			gainNode->params[Gain::GAINMOD_ID] = gainModId;
			m_graph.insert_edge(gainId, gainModId);
			m_graph.insert_edge(gainId, gainInId);
			m_nodes.push_back(gainNode);
			const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
			ImNodes::SetNodeScreenSpacePos(gainId, click_pos);
		}
		else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_X)) {
			auto mix = std::make_shared<QuadMixer>();
			auto a = std::make_shared<Value>(0.f);
			auto b = std::make_shared<Value>(0.f);
			auto c = std::make_shared<Value>(0.f);
			auto d = std::make_shared<Value>(0.f);
			auto aId = m_graph.insert_node(a);
			auto bId = m_graph.insert_node(b);
			auto cId = m_graph.insert_node(c);
			auto dId = m_graph.insert_node(d);
			auto mixId = m_graph.insert_node(mix);
			m_graph.insert_edge(mixId, dId);
			m_graph.insert_edge(mixId, cId);
			m_graph.insert_edge(mixId, bId);
			m_graph.insert_edge(mixId, aId);
			mix->params[QuadMixer::NODE_ID] = mixId;
			mix->params[QuadMixer::INPUT_A_ID] = aId;
			mix->params[QuadMixer::INPUT_B_ID] = bId;
			mix->params[QuadMixer::INPUT_C_ID] = cId;
			mix->params[QuadMixer::INPUT_D_ID] = dId;
			m_nodes.push_back(mix);
			const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
			ImNodes::SetNodeScreenSpacePos(mixId, click_pos);
		}


		else if (ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_S)) {
			auto outputNode = std::make_shared<Output>();
			auto leftNode = std::make_shared<Value>(0.f);
			auto rightNode = std::make_shared<Value>(0.f);
			auto rightNodeId = m_graph.insert_node(rightNode);
			auto leftNodeId = m_graph.insert_node(leftNode);
			auto outputId = m_graph.insert_node(outputNode);
			m_graph.insert_edge(outputId, rightNodeId);
			m_graph.insert_edge(outputId, leftNodeId);
			outputNode->params[Output::NODE_ID] = outputId;
			outputNode->params[Output::INPUT_R_ID] = rightNodeId;
			outputNode->params[Output::INPUT_L_ID] = leftNodeId;
			m_nodes.push_back(outputNode);
			const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
			ImNodes::SetNodeScreenSpacePos(outputId, click_pos);
			m_rootNodeId = outputId;
			m_graph.setRoot(m_rootNodeId);
		}

    }

	for (auto& node : m_nodes) {
		// with overriding, no stupid case-switch needed
		node->display();
	}

	for (const auto& edge : m_graph.edges()) {
		// If edge doesn't start at value, then it's an internal edge, i.e.
		// an edge which links a node's operation to its input. We don't
		// want to render node internals with visible links.
		auto node = m_graph.node(edge.from);
		if (node->type != NodeType::VALUE) {
			continue;
		}

		ImNodes::Link(edge.id, edge.from, edge.to);
	}


    ImNodes::EndNodeEditor();

	{
		int startAttr = 0;
		int endAttr = 0;
		bool bSnap = false;
		if (ImNodes::IsLinkCreated(&startAttr, &endAttr, &bSnap)) {
			const NodeType startType = m_graph.node(startAttr)->type;
			const NodeType endType = m_graph.node(endAttr)->type;

			const bool bLinkValid = (startType != endType);
			if (bLinkValid) {
				if (startType != NodeType::VALUE) {
					std::swap(startAttr, endAttr);
				}
				m_graph.insert_edge(startAttr, endAttr);
			}
		}
	}

    ImGui::End();
}

