#include "model.h"
#include "util.h"

#include "gain.h"
#include "value.h"

Model::Model()
{
	m_creators[NodeType::GAIN] = std::make_shared<GainNodeCreator>(m_graph, m_cache);
}

void Model::link(int from, int to)
{
	if (m_cache.map.find(from) != m_cache.map.end()) {
		m_cache.map[from].edges[from] = EdgeSnapshot(from, to);
	}
	m_graph.insert_edge(from, to);
}

int Model::update(UpdateEvent update)
{
	auto i = update.nodeId;
	auto p = update.parameter;
	auto v = update.value;
	auto node = m_graph.node(i);
	// no controller should be able to update id's
	auto p_idx = node->lookupParam(p);
	// todo: restrict to valid values for a given param
	if (p_idx != -1) {
		node->params[p_idx] = update.value;
		m_cache.map[i].params[p] = v;
	}
	return 0;
}

int Model::create(NodeType type)
{
	auto command = m_creators[type];
	return command->create();
}

int GainNodeCreator::create()
{
	auto gainNode = std::make_shared<Gain>();
	auto gainIn = std::make_shared<Value>(0.f);
	auto gainMod = std::make_shared<Value>(1.f);
	auto gainModId = m_g.insert_node(gainMod);
	auto gainInId = m_g.insert_node(gainIn);
	auto gainId = m_g.insert_node(gainNode);
	gainNode->params[Gain::NODE_ID] = gainId;
	gainNode->params[Gain::INPUT_ID] = gainInId;
	gainNode->params[Gain::GAINMOD_ID] = gainModId;
	m_g.insert_edge(gainId, gainModId);
	m_g.insert_edge(gainId, gainInId);
	cacheType(gainInId, NodeType::VALUE);
	cacheType(gainModId, NodeType::VALUE);
	cacheType(gainId, NodeType::GAIN);
	cacheParam(gainId, "input_id", gainInId);
	cacheParam(gainId, "gainmod_id", gainModId);
	cacheParam(gainId, "gain", 0.f);

	return gainId;
}

void NodeCreationCommand::cacheType(int id, NodeType t)
{
	if (m_v.map.find(id) == m_v.map.end()) {
		NodeSnapshot snap;
		m_v.map[id] = snap;
	}

	m_v.map[id].nodeType = t;
}

void NodeCreationCommand::cacheParam(int id, std::string param, float value) {
	if (m_v.map.find(id) == m_v.map.end()) {
		NodeSnapshot snap;
		m_v.map[id] = snap;
	}

	m_v.map[id].params[param] = value;

}
