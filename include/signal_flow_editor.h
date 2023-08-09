#pragma once

#include "imnodes.h"
#include "node.h"
#include "nodetypes.h"
#include "graph.h"
#include <vector>
#include <memory>

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

private:
	ImNodesEditorContext* m_pContext;
	Graph<Node> m_graph;
	std::vector<std::shared_ptr<Node>> m_nodes; // need pointers to call process() and display()
	std::vector<Link> m_links;
	int m_currentId;
	int m_rootNodeId;

};



