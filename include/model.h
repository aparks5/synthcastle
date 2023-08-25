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
	std::pair<float,float> evaluate();
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

class ConstantNodeCreator : public NodeCreationCommand
{
public:
	ConstantNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~ConstantNodeCreator() {};
	int create() override;
};

class OutputNodeCreator : public NodeCreationCommand
{
public:
	OutputNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~OutputNodeCreator() {};
	int create() override;
};

class OscillatorNodeCreator : public NodeCreationCommand
{
public:
	OscillatorNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~OscillatorNodeCreator() {};
	int create() override;
};

class FilterNodeCreator : public NodeCreationCommand
{
public:
	FilterNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~FilterNodeCreator() {};
	int create() override;
};

class EnvelopeNodeCreator : public NodeCreationCommand
{
public:
	EnvelopeNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~EnvelopeNodeCreator() {};
	int create() override;
};

class TrigNodeCreator : public NodeCreationCommand
{
public:
	TrigNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~TrigNodeCreator() {};
	int create() override;
};


class Model
{
public:
	Model();
	virtual ~Model() {}
	int update(UpdateEvent update);
	int create(std::string str);
	void link(int from, int to);
	std::tuple<float, float> evaluate();
	const ViewBag refresh() { return m_cache; }


private:
	std::unordered_map<NodeType, std::shared_ptr<NodeCreationCommand>> m_creators;
	NodeGraph m_graph;
	ViewBag m_cache;

	// unnecessary, every node type should have a unique name
	std::unordered_map<std::string, NodeType> m_nodeTypeMap =
	{
		{"gain", NodeType::GAIN},
		{"oscillator", NodeType::OSCILLATOR},
		{"constant", NodeType::CONSTANT},
		{"output", NodeType::OUTPUT},
		{"filter", NodeType::FILTER},
		{"envelope", NodeType::ENVELOPE},
		{"trig", NodeType::TRIG}
	};

};