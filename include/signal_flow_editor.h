#pragma once

#include "imnodes.h"
#include "node.h"
#include "nodetypes.h"
#include "nodegraph.h"
#include <vector>
#include <memory>
#include <mutex>

#define INVALID_PARAM_VALUE (-128)

class SignalFlowEditor
{
public:
	SignalFlowEditor();
	virtual ~SignalFlowEditor();
	struct Link
	{
		int id;
		int start_attr;
		int end_attr;
	};

	void show();
	void shutdown();
	float output();
	int root() { return m_rootNodeId; }
	const NodeGraph* graph() const; // returns a reference, not thread safe
	
private:
	void save();
	void load();
	std::mutex m_mut;
	ImNodesEditorContext* m_pContext;
	NodeGraph m_graph;
	std::vector<std::shared_ptr<Node>> m_nodes; // need pointers to call process() and display()
	std::vector<Link> m_links;
	int m_voiceCount;
	int m_currentId;
	int m_rootNodeId;
	float evaluate(const NodeGraph& graph, const int root_node);

};



