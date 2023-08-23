#pragma once

#include "nodegraph.h"
#include "viewbag.h"
#include "events.h"

class NodeCreationCommand {
public:
	NodeCreationCommand(NodeGraph& g,
		ViewBag& v)
		: m_g(g)
		, m_v(v) {}
	virtual ~NodeCreationCommand() {};
	virtual int create() = 0;
protected:
	NodeGraph& m_g;
	ViewBag& m_v;
	void cacheParam(int id, std::string param, float value);
	void cacheType(int id, NodeType t);
};

class GainNodeCreator : public NodeCreationCommand
{
public:
	GainNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~GainNodeCreator() {};
	int create() override;
};

class Model
{
public:
	Model();
	virtual ~Model() {}
	int update(UpdateEvent update);
	int create(NodeType type);
	void link(int from, int to);
	const ViewBag refresh() { return m_cache; }

private:
	std::unordered_map<NodeType, std::shared_ptr<NodeCreationCommand>> m_creators;
	NodeGraph m_graph;
	ViewBag m_cache;
};