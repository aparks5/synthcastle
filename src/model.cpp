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
	// you need to assign this gain id for linking
	auto gainModId = m_g.insert_node(gainMod);
	auto gainInId = m_g.insert_node(gainIn);
	auto gainId = m_g.insert_node(gainNode);
	gainNode->params[Gain::NODE_ID] = gainId;
	gainNode->params[Gain::INPUT_ID] = gainInId;
	gainNode->params[Gain::GAINMOD_ID] = gainModId;
	m_g.insert_edge(gainId, gainModId);
	m_g.insert_edge(gainId, gainInId);
	NodeSnapshot snap;
	m_v.map[gainInId] = snap;
	m_v.map[gainInId].nodeType = NodeType::VALUE;
	m_v.map[gainInId].edges[0] = EdgeSnapshot(gainId, gainInId);
	m_v.map[gainModId] = snap;
	m_v.map[gainModId].nodeType = NodeType::VALUE;
	m_v.map[gainId] = snap;
	m_v.map[gainId].nodeType = NodeType::GAIN;
	m_v.map[gainId].params["input_id"] = gainInId;
	m_v.map[gainId].params["gainmod_id"] = gainModId;
	m_v.map[gainId].edges[0] = EdgeSnapshot(gainId, gainModId);
	m_v.map[gainId].edges[1] = EdgeSnapshot(gainId, gainInId);
	m_v.map[gainId].params["gain"] = 0.f;

	return gainId;
}
