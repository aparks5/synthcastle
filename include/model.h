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
	NodeCreationCommand(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: m_g(g)
		, m_v(v) {}
	virtual ~NodeCreationCommand() {};
	virtual int create() = 0;
protected:
	std::shared_ptr<NodeGraph> m_g;
	ViewBag& m_v;
	void cacheParam(int id, std::string param, float value);
	void cacheString(int id, std::string param, std::string str);
	void cacheType(int id, NodeType t);
	std::pair<float,float> evaluate();
};

class GainNodeCreator : public NodeCreationCommand
{
public:
	GainNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~GainNodeCreator() {};
	int create() override;
};

class SamplerNodeCreator : public NodeCreationCommand
{
public:
	SamplerNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~SamplerNodeCreator() {};
	int create() override;
};

class ConstantNodeCreator : public NodeCreationCommand
{
public:
	ConstantNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~ConstantNodeCreator() {};
	int create() override;
};

class DistortNodeCreator : public NodeCreationCommand
{
public:
	DistortNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~DistortNodeCreator() {};
	int create() override;
};

class AudioInputNodeCreator : public NodeCreationCommand
{
public:
	AudioInputNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~AudioInputNodeCreator() {};
	int create() override;
};

class OutputNodeCreator : public NodeCreationCommand
{
public:
	OutputNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~OutputNodeCreator() {};
	int create() override;
};

class OscillatorNodeCreator : public NodeCreationCommand
{
public:
	OscillatorNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~OscillatorNodeCreator() {};
	int create() override;
};

class FilterNodeCreator : public NodeCreationCommand
{
public:
	FilterNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~FilterNodeCreator() {};
	int create() override;
};

class EnvelopeNodeCreator : public NodeCreationCommand
{
public:
	EnvelopeNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~EnvelopeNodeCreator() {};
	int create() override;
};

class MixerNodeCreator : public NodeCreationCommand
{
public:
	MixerNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~MixerNodeCreator() {};
	int create() override;
};

class TrigNodeCreator : public NodeCreationCommand
{
public:
	TrigNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~TrigNodeCreator() {};
	int create() override;
};

class SeqNodeCreator : public NodeCreationCommand
{
public:
	SeqNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
		: NodeCreationCommand(g, v)
	{}
	virtual ~SeqNodeCreator() {};
	int create() override;
};

class DelayNodeCreator : public NodeCreationCommand
{
public:
	DelayNodeCreator(std::shared_ptr<NodeGraph> g, ViewBag& v)
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
	std::vector<std::string> queryNodeNames() const;
	std::tuple<float, float> evaluate();
	const ViewBag refresh(); 
	// return a deep copy of the graph
	NodeGraph cloneGraph();

private:
	std::shared_ptr<NodeGraph> m_graph;
	std::unordered_map<NodeType, std::shared_ptr<NodeCreationCommand>> m_creators;
	ViewBag m_cache;

	// unnecessary, every node type should have a unique name
	std::unordered_map<std::string, NodeType> m_nodeTypeMap =
	{
		{"audio_input", NodeType::AUDIO_IN},
		{"constant", NodeType::CONSTANT},
		{"delay", NodeType::DELAY},
		{"distort", NodeType::DISTORT},
		{"envelope", NodeType::ENVELOPE},
		{"filter", NodeType::FILTER},
		{"gain", NodeType::GAIN},
		{"mixer", NodeType::QUAD_MIXER},
		{"oscillator", NodeType::OSCILLATOR},
		{"output", NodeType::OUTPUT},
		{"sampler", NodeType::SAMPLER},
		{"seq", NodeType::SEQ},
		{"trig", NodeType::TRIG},
	};

};