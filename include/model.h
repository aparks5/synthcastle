#pragma once

#include "nodegraph.h"
#include "viewbag.h"
#include "events.h"

struct spin_mutex {
	void lock() noexcept {
		while (!try_lock()) {
			flag.wait(true, std::memory_order_relaxed);
		}
	}

	bool try_lock() noexcept {
		return !flag.test_and_set(std::memory_order_acquire);
	}

	void unlock() noexcept {
		flag.clear(std::memory_order_release);
		flag.notify_one();
	}

private:
	std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

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
	void cacheString(int id, std::string param, std::string str);
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

class SamplerNodeCreator : public NodeCreationCommand
{
public:
	SamplerNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~SamplerNodeCreator() {};
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

class MixerNodeCreator : public NodeCreationCommand
{
public:
	MixerNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~MixerNodeCreator() {};
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

class SeqNodeCreator : public NodeCreationCommand
{
public:
	SeqNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~SeqNodeCreator() {};
	int create() override;
};

class DelayNodeCreator : public NodeCreationCommand
{
public:
	DelayNodeCreator(NodeGraph& g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~DelayNodeCreator() {};
	int create() override;
};

class Model
{
public:
	Model();
	virtual ~Model() {}
	int update(UpdateEvent update);
	int update(UpdateStringEvent update);
	int create(std::string str);
	void link(int from, int to);
	std::tuple<float, float> evaluate();
	const ViewBag refresh(); 

private:
	spin_mutex m_mut;
	std::unordered_map<NodeType, std::shared_ptr<NodeCreationCommand>> m_creators;
	NodeGraph m_graph;
	ViewBag m_cache;

	// unnecessary, every node type should have a unique name
	std::unordered_map<std::string, NodeType> m_nodeTypeMap =
	{
		{"gain", NodeType::GAIN},
		{"delay", NodeType::DELAY},
		{"oscillator", NodeType::OSCILLATOR},
		{"constant", NodeType::CONSTANT},
		{"output", NodeType::OUTPUT},
		{"filter", NodeType::FILTER},
		{"mixer", NodeType::QUAD_MIXER},
		{"envelope", NodeType::ENVELOPE},
		{"trig", NodeType::TRIG},
		{"sampler", NodeType::SAMPLER},
		{"seq", NodeType::SEQ}
	};

};