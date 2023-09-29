#include "model.h"
#include "util.h"

#include <algorithm>

#include "audioinput.h"
#include "constant.h"
#include "delay.h"
#include "distort.h"
#include "envelope.h"
#include "filter.h"
#include "freqshift.h"
#include "gain.h"
#include "looper.h"
#include "midi.h"
#include "oscillator.h"
#include "output.h"
#include "quadmixer.h"
#include "relay.h"
#include "sampler.h"
#include "seq.h"
#include "trig.h"
#include "value.h"

#define INVALID_PARAM_VALUE (-128)

// TODO: replace with class registry pattern
class ProcessorNodeFactory
{
public:
	ProcessorNodeFactory();
	virtual ~ProcessorNodeFactory();
	// this factory will grow without Registry pattern
	static std::shared_ptr<Node> create(std::string str) {
		if (str == "audio_input") {
			return std::make_shared<AudioInput>();
		}
		else if (str == "constant") {
			return std::make_shared<Constant>();
		}
		else if (str == "distort") {
			return std::make_shared<Distort>();
		}
		else if (str == "delay") {
			return std::make_shared<Delay>();
		}
		else if (str == "envelope") {
			return std::make_shared<Envelope>();
		}
		else if (str == "filter") {
			return std::make_shared<Filter>();
		}
		else if (str == "freqshift") {
			return std::make_shared<FrequencyShifter>();
		}
		else if (str == "gain") {
			return std::make_shared<Gain>();
		}
		else if (str == "looper") {
			return std::make_shared<Looper>();
		}
		else if (str == "mixer") {
			return std::make_shared<QuadMixer>();
		}
		else if (str == "oscillator") {
			return std::make_shared<Oscillator>();
		}
		else if (str == "output") {
			return std::make_shared<Output>();
		}
		else if (str == "sampler") {
			return std::make_shared<Sampler>();
		}
		else if (str == "seq") {
			return std::make_shared<Seq>();
		}
		else if (str == "trig") {
			return std::make_shared<Trig>();
		}
		else if (str == "midi") {
			return std::make_shared<MIDI>();
		}

		// we should never reach here
		return std::make_shared<Constant>();
	}
};

Model::Model()
	: m_graph(std::make_shared<NodeGraph>())
	, nodeCreator(m_graph, m_cache)
{}

NodeGraph Model::cloneGraph()
{
	NodeGraph graph(*m_graph);
	return graph;
}

std::vector<std::string> Model::queryNodeNames() const
{
	return names;
}

ViewBag Model::refresh()
{
	std::stack<int> postorder;
	if (m_graph->getRoot() != -1) {
		dfs_traverse(m_graph, [&postorder](const int node_id) -> void { postorder.push(node_id); });

		// traverse all nodes and update the cache with all the params
		// after evaluate() process() can update any number of internal params
		// we need to refresh the cache with these in order to reflect them
		// in the UI
		while (!postorder.empty()) {
			const int id = postorder.top();
			postorder.pop();
			const auto& pNode = m_graph->node(id);
			auto paramStrings = pNode->paramStrings();
			for (auto& str : paramStrings) {
				// TODO: repeat this for inputIndexByName and outputIndexByName?
				// maybe not necessary since inputs are updated at construction, id's are unchanging
				// and inputs and outputs are modified for every sample
				auto param = pNode->paramIndexByName(str);
				auto value = pNode->params[param];
				if (m_cache.map.find(id) != m_cache.map.end()) {
					m_cache.map[id].params[str] = value;
				}
			}
		}
	}

	return m_cache;
}

void Model::link(int from, int to)
{
	auto id = m_graph->insert_edge(from, to);

	if (m_cache.map.find(from) != m_cache.map.end()) {
		m_cache.map[from].edges[id] = EdgeSnapshot(from, to);
	}
}

void Model::deleteLink(int link_id)
{
	// this has a terrible worst case searching for the node that contains this link
	// but its the price we pay for using a cache
	for (auto const& [id, snap] : m_cache.map) {
		if (snap.edges.find(link_id) != snap.edges.end()) {
			m_cache.map[id].edges.erase(link_id);
		}
	}

	m_graph->erase_edge(link_id);
}

int Model::update(UpdateEvent update)
{
	auto i = update.nodeId;
	auto p = update.parameter;
	auto v = update.value;
	auto node = m_graph->node(i);
	// no controller should be able to update id's
	auto p_idx = node->paramIndexByName(p);
	if (p_idx != -1) {
		node->params[p_idx] = update.value;
		m_cache.map[i].params[p] = v;
	}

	return 0;
}

int Model::update(UpdateStringEvent update)
{
	auto i = update.nodeId;
	auto p = update.parameter;
	auto v = update.value;
	auto node = m_graph->node(i);

	int ok = -1;
	if (node->stringParams.find(p) != node->stringParams.end()) {
		node->stringParams[p] = v;
		m_cache.map[i].stringParams[p] = v;
		ok = 0;
	}

	return ok;
}

// NOTE: this method is important. it creates the nodes from enums and lists in the Node definition
int ProcessorNodeCreator::create(std::string str)
{
	auto processorNode = ProcessorNodeFactory::create(str);

	std::vector<std::shared_ptr<ProcessorInput>> inputNodes;

	for (auto& in : processorNode->inputs) {
		auto inputNode = std::make_shared<ProcessorInput>();
		inputNodes.push_back(inputNode);
	}

	// insert inputs in reverse order
	std::stack<int> inputNodeIds;
	for (auto& in : inputNodes) {
		auto id = m_g->insert_node(in);
		inputNodeIds.push(id);
	}

	auto processorNodeId = m_g->insert_node(processorNode); 

	if (processorNode->getName() == "output") {
		m_g->setRoot(processorNodeId);
	}

	cacheType(processorNodeId, NodeType::PROCESSOR);
	cacheName(processorNodeId, processorNode->getName());

	for (auto& str : processorNode->inputStrings()) {
		cacheInput(processorNodeId, str, 0.f);
	}

	// edges also need reverse order
	int inp_id = inputNodeIds.size() - 1;
	while (!inputNodeIds.empty()) {
		auto id = inputNodeIds.top();
		inputNodeIds.pop();
		m_g->insert_edge(processorNodeId, id);
		cacheType(id, NodeType::PROCESSOR_INPUT);
		auto str = processorNode->inputIdToString(inp_id--);
		cacheInput(processorNodeId, str, id);
	}

	std::vector<std::shared_ptr<ProcessorOutput>> outputNodes;
	for (size_t idx = 0; idx < processorNode->outputs.size(); idx++) {
		auto outputNode = std::make_shared<ProcessorOutput>(idx);
		outputNodes.push_back(outputNode);
	}

	std::vector<int> outputNodeIds;
	for (auto& outputNode : outputNodes) {
		auto id = m_g->insert_node(outputNode);
		outputNodeIds.push_back(id);
	}

	for (auto& str : processorNode->outputStrings()) {
		cacheOutput(processorNodeId, str, 0.f);
	}

	int out = 0;
	for (auto& outputNodeId : outputNodeIds) {
		m_g->insert_edge(outputNodeId, processorNodeId);
		cacheType(outputNodeId, NodeType::PROCESSOR_OUTPUT);
		auto str = processorNode->outputIdToString(out++);
		cacheOutput(processorNodeId, str, outputNodeId);
	}

	for (auto& str : processorNode->paramStrings()) {
		cacheParam(processorNodeId, str, 0.f);
	}

	for (auto& [k,v] : processorNode->stringParams) {
		cacheString(processorNodeId, k, v);
	}

	cacheParam(processorNodeId, "node_id", processorNodeId);
	return processorNodeId;
}

int Model::create(std::string str)
{
	return nodeCreator.create(str);
}

void ProcessorNodeCreator::cacheType(int id, NodeType t)
{
	if (m_v.map.find(id) == m_v.map.end()) {
		NodeSnapshot snap;
		m_v.map[id] = snap;
	}

	m_v.map[id].nodeType = t;
}

void ProcessorNodeCreator::cacheName(int id, std::string str)
{
	if (m_v.map.find(id) == m_v.map.end()) {
		NodeSnapshot snap;
		m_v.map[id] = snap;
	}

	m_v.map[id].name = str;
}

void ProcessorNodeCreator::cacheParam(int id, std::string param, float value)
{
	m_v.map[id].params[param] = value;
}

void ProcessorNodeCreator::cacheInput(int id, std::string in, float value)
{
	m_v.map[id].inputs[in] = value;
}

void ProcessorNodeCreator::cacheOutput(int id, std::string out, float value)
{
	m_v.map[id].outputs[out] = value;
}

void ProcessorNodeCreator::cacheString(int id, std::string param, std::string str) {
	if (m_v.map.find(id) == m_v.map.end()) {
		NodeSnapshot snap;
		m_v.map[id] = snap;
	}
	m_v.map[id].stringParams[param] = str;

}
