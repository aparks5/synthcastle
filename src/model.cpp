#include "model.h"
#include "util.h"

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

Model::Model()
{
	m_graph = std::make_shared<NodeGraph>();

	m_creators[NodeType::AUDIO_IN] = std::make_shared<AudioInputNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::CONSTANT] = std::make_shared<ConstantNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::DELAY] = std::make_shared<DelayNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::DISTORT] = std::make_shared<DistortNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::ENVELOPE] = std::make_shared<EnvelopeNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::FILTER] = std::make_shared<FilterNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::FREQ_SHIFT] = std::make_shared<FreqShiftNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::GAIN] = std::make_shared<GainNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::LOOPER] = std::make_shared<LooperNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::MIDI_IN] = std::make_shared<MidiInputNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::OSCILLATOR] = std::make_shared<OscillatorNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::OUTPUT] = std::make_shared<OutputNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::QUAD_MIXER] = std::make_shared<MixerNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::SAMPLER] = std::make_shared<SamplerNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::SEQ] = std::make_shared<SeqNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::TRIG] = std::make_shared<TrigNodeCreator>(m_graph, m_cache);
}

NodeGraph Model::cloneGraph()
{
	NodeGraph graph(*m_graph);
	return graph;

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
				auto param = pNode->lookupParam(str);
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

	// changed, edges are now indexed by id
	if (m_cache.map.find(from) != m_cache.map.end()) {
		m_cache.map[from].edges[id] = EdgeSnapshot(from, to);
	}
}

void Model::deleteLink(int link_id)
{
	// i dont think the cache knows about link ids 
	// but it needs to know if wants to delete edges

	// how do i find the map index
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
	auto p_idx = node->lookupParam(p);
	// todo: restrict to valid values for a given param
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
	// todo: restrict to valid values for a given param
	if (node->stringParams.find(p) != node->stringParams.end()) {
		node->stringParams[p] = v;
		m_cache.map[i].stringParams[p] = v;
	}

	return 0;
}

std::vector<std::string> Model::queryNodeNames() const 
{
	std::vector<std::string> names;
	for (auto& kv : m_nodeTypeMap) {
		names.push_back(kv.first);
	}

	return names;
}

int Model::create(std::string str)
{
	if (m_nodeTypeMap.find(str) != m_nodeTypeMap.end()) {
		NodeType type = m_nodeTypeMap[str];
		auto command = m_creators[type];
		return command->create();
	}

	return -1;
}

int ConstantNodeCreator::create()
{
	// SO MUCH REPETITION AHHHH NEEDS REFACTORING
	auto k = std::make_shared<Constant>();
	auto in1 = std::make_shared<Value>();
	auto in2 = std::make_shared<Value>();
	auto in3 = std::make_shared<Value>();
	auto in4 = std::make_shared<Value>();
	auto out1 = std::make_shared<Relay>(0);
	auto out2 = std::make_shared<Relay>(1);
	auto out3 = std::make_shared<Relay>(2);
	auto out4 = std::make_shared<Relay>(3);
	// insert inputs in reverse order
	auto in4id = m_g->insert_node(in4);
	auto in3id = m_g->insert_node(in3);
	auto in2id = m_g->insert_node(in2);
	auto in1id = m_g->insert_node(in1);
	auto id = m_g->insert_node(k); // outputs inserted AFTER process
	// first out gen'd auto
	auto out1id = m_g->insert_node(out1);
	auto out2id = m_g->insert_node(out2);
	auto out3id = m_g->insert_node(out3);
	auto out4id = m_g->insert_node(out4);
	m_g->insert_edge(id, in4id); // outputs in reverse order?
	m_g->insert_edge(id, in3id); // outputs in reverse order?
	m_g->insert_edge(id, in2id); // outputs in reverse order?
	m_g->insert_edge(id, in1id); // outputs in reverse order?
	m_g->insert_edge(out1id, id);
	m_g->insert_edge(out2id, id);
	m_g->insert_edge(out3id, id);
	m_g->insert_edge(out4id, id);
	k->params[Constant::NODE_ID] = id;
	k->params[Constant::INPUT1_ID] = in1id;
	k->params[Constant::INPUT2_ID] = in2id;
	k->params[Constant::INPUT3_ID] = in3id;
	k->params[Constant::INPUT4_ID] = in4id;
	k->params[Constant::OUTPUT1_ID] = out1id;
	k->params[Constant::OUTPUT2_ID] = out2id;
	k->params[Constant::OUTPUT3_ID] = out3id;
	k->params[Constant::OUTPUT4_ID] = out4id;
	cacheType(id, NodeType::CONSTANT);
	cacheType(in1id, NodeType::VALUE);
	cacheType(in2id, NodeType::VALUE);
	cacheType(in3id, NodeType::VALUE);
	cacheType(in4id, NodeType::VALUE);
	cacheType(out1id, NodeType::RELAY);
	cacheType(out2id, NodeType::RELAY);
	cacheType(out3id, NodeType::RELAY);
	cacheType(out4id, NodeType::RELAY);
	for (auto& str : k->paramStrings()) {
		cacheParam(id, str, 0.f);
	}

	cacheParam(id, "node_id", id);
	cacheParam(id, "input1_id", in1id);
	cacheParam(id, "input2_id", in2id);
	cacheParam(id, "input3_id", in3id);
	cacheParam(id, "input4_id", in4id);
	cacheParam(id, "output1_id", out1id);
	cacheParam(id, "output2_id", out2id);
	cacheParam(id, "output3_id", out3id);
	cacheParam(id, "output4_id", out4id);

	return id;
}

int DistortNodeCreator::create()
{
	auto k = std::make_shared<Distort>();
	auto in = std::make_shared<Value>();
	auto inid = m_g->insert_node(in);
	auto id = m_g->insert_node(k);
	m_g->insert_edge(id, inid);
	k->params[Distort::NODE_ID] = id;
	k->params[Distort::INPUT_ID] = inid;
	cacheType(id, NodeType::DISTORT);
	cacheType(inid, NodeType::VALUE);
	for (auto& str : k->paramStrings()) {
		cacheParam(id, str, 0.f);
	}
	cacheParam(id, "node_id", id);
	cacheParam(id, "input_id", inid);
	return id;
}

int FreqShiftNodeCreator::create()
{
	auto k = std::make_shared<FrequencyShifter>();
	auto in = std::make_shared<Value>();
	auto freq = std::make_shared<Value>();
	// when adding outputs we should simply increment the relay index
	auto out = std::make_shared<Relay>(0);

	auto freqid = m_g->insert_node(freq);
	auto inid = m_g->insert_node(in);
	auto id = m_g->insert_node(k);
	auto outid = m_g->insert_node(out);

	m_g->insert_edge(id, freqid);
	m_g->insert_edge(id, inid);
	m_g->insert_edge(outid, id);

	k->params[FrequencyShifter::NODE_ID] = id;
	k->params[FrequencyShifter::INPUT_ID] = inid;
	k->params[FrequencyShifter::FREQ_ID] = freqid;
	k->params[FrequencyShifter::OUTPUT_ID] = outid;

	cacheType(id, NodeType::FREQ_SHIFT);
	cacheType(inid, NodeType::VALUE);
	cacheType(freqid, NodeType::VALUE);
	cacheType(outid, NodeType::RELAY);

	for (auto& str : k->paramStrings()) {
		cacheParam(id, str, 0.f);
	}
	cacheParam(id, "node_id", id);
	cacheParam(id, "freq_id", freqid);
	cacheParam(id, "input_id", inid);
	cacheParam(id, "output_id", outid);

	return id;

}

int AudioInputNodeCreator::create()
{
	auto k = std::make_shared<AudioInput>();
	auto id = m_g->insert_node(k);
	k->params[Constant::NODE_ID] = id;
	cacheType(id, NodeType::AUDIO_IN);
	return id;
}

int MidiInputNodeCreator::create()
{
	auto k = std::make_shared<MIDI>();
	// for auto & output : k->outputs();
	// std::make_shared
	// insert node into map with id
	// assign out ids
	// cache as relays
	auto v1 = std::make_shared<Relay>(0);
	auto v2 = std::make_shared<Relay>(1);
	auto v3 = std::make_shared<Relay>(2);
	auto v4 = std::make_shared<Relay>(3);
	auto vel = std::make_shared<Relay>(4);
	auto id = m_g->insert_node(k);
	auto v1id = m_g->insert_node(v1);
	auto v2id = m_g->insert_node(v2);
	auto v3id = m_g->insert_node(v3);
	auto v4id = m_g->insert_node(v4);
	auto velid = m_g->insert_node(vel);
	m_g->insert_edge(v1id, id);
	m_g->insert_edge(v2id, id);
	m_g->insert_edge(v3id, id);
	m_g->insert_edge(v4id, id);
	m_g->insert_edge(velid, id);
	k->params[MIDI::NODE_ID] = id;
	k->params[MIDI::OUT_VOICE1_ID] = v1id;
	k->params[MIDI::OUT_VOICE2_ID] = v2id;
	k->params[MIDI::OUT_VOICE3_ID] = v3id;
	k->params[MIDI::OUT_VOICE4_ID] = v4id;
	k->params[MIDI::OUT_VELOCITY_ID] = velid;
	cacheType(id, NodeType::MIDI_IN);
	cacheType(v1id, NodeType::RELAY);
	cacheType(v2id, NodeType::RELAY);
	cacheType(v3id, NodeType::RELAY);
	cacheType(v4id, NodeType::RELAY);
	cacheType(velid, NodeType::RELAY);

	for (auto& str : k->paramStrings()) {
		cacheParam(id, str, 0.f);
	}

	cacheParam(id, "num_ports", k->params[MIDI::NUM_PORTS]);

	cacheString(id, "portname", "");

	cacheParam(id, "node_id", id);
	cacheParam(id, "out_voice1_id", v1id);
	cacheParam(id, "out_voice2_id", v2id);
	cacheParam(id, "out_voice3_id", v3id);
	cacheParam(id, "out_voice4_id", v4id);
	cacheParam(id, "out_velocity_id", velid);

	return id;
}

int LooperNodeCreator::create()
{
	auto k = std::make_shared<Looper>();
	auto in = std::make_shared<Value>();
	auto inid = m_g->insert_node(in);
	auto id = m_g->insert_node(k);
	k->params[Looper::NODE_ID] = id;
	k->params[Looper::INPUT_ID] = inid;
	m_g->insert_edge(id, inid);
	cacheType(id, NodeType::LOOPER);
	cacheType(inid, NodeType::VALUE);

	for (auto& str : k->paramStrings()) {
		cacheParam(id, str, 0.f);
	}

	cacheParam(id, "node_id", id);
	cacheParam(id, "input_id", inid);
	cacheString(id, "path", "");
	return id;
}

int SamplerNodeCreator::create()
{
	auto k = std::make_shared<Sampler>();
	auto in = std::make_shared<Value>();
	auto pos = std::make_shared<Value>();
	auto startstop = std::make_shared<Value>();
	auto inid = m_g->insert_node(in);
	auto posid = m_g->insert_node(pos);
	auto startid = m_g->insert_node(startstop);
	auto id = m_g->insert_node(k);
	k->params[Sampler::NODE_ID] = id;
	k->params[Sampler::PITCH_ID] = inid;
	k->params[Sampler::POSITION_ID] = posid;
	k->params[Sampler::STARTSTOP_ID] = startid;
	m_g->insert_edge(id, startid);
	m_g->insert_edge(id, posid);
	m_g->insert_edge(id, inid);
	cacheType(id, NodeType::SAMPLER);
	cacheType(inid, NodeType::VALUE);
	cacheType(posid, NodeType::VALUE);
	cacheType(startid, NodeType::VALUE);

	for (auto& str : k->paramStrings()) {
		cacheParam(id, str, 0.f);
	}
	cacheParam(id, "node_id", id);
	cacheParam(id, "pitch_id", inid);
	cacheParam(id, "position_id", posid);
	cacheParam(id, "startstop_id", startid);
	cacheString(id, "path", "");
	return id;
}

int TrigNodeCreator::create()
{
	auto k = std::make_shared<Trig>();
	auto id = m_g->insert_node(k);
	k->params[Trig::NODE_ID] = id;
	cacheType(id, NodeType::TRIG);
	cacheParam(id, "bpm", 0.f);
	cacheParam(id, "trig", 0.f);
	cacheParam(id, "value", 0.f);
	cacheParam(id, "progress", 0.f);
	cacheParam(id, "progress_dir", 1.f);
	cacheParam(id, "stop", 0.f);
	cacheParam(id, "numtrigs", k->params[Trig::NUMTRIGS]);
	for (size_t idx = 2; idx <= k->params[Trig::NUMTRIGS]; idx++) {
		std::string str = "trig" + std::to_string(idx);
		str += "_id";
		auto temp = std::make_shared<Relay>(idx);
		auto tempId = m_g->insert_node(temp);
		k->params[Trig::TRIG1_ID + idx - 1] = tempId;
		m_g->insert_edge(tempId, id);
		cacheParam(id, str, tempId);
	}
	return id;
}

int MixerNodeCreator::create()
{
	auto mix = std::make_shared<QuadMixer>();
	auto a = std::make_shared<Value>(0.f);
	auto b = std::make_shared<Value>(0.f);
	auto c = std::make_shared<Value>(0.f);
	auto d = std::make_shared<Value>(0.f);
	auto aId = m_g->insert_node(a);
	auto bId = m_g->insert_node(b);
	auto cId = m_g->insert_node(c);
	auto dId = m_g->insert_node(d);
	auto id = m_g->insert_node(mix);
	m_g->insert_edge(id, dId);
	m_g->insert_edge(id, cId);
	m_g->insert_edge(id, bId);
	m_g->insert_edge(id, aId);
	mix->params[QuadMixer::NODE_ID] = id;
	mix->params[QuadMixer::INPUT_A_ID] = aId;
	mix->params[QuadMixer::INPUT_B_ID] = bId;
	mix->params[QuadMixer::INPUT_C_ID] = cId;
	mix->params[QuadMixer::INPUT_D_ID] = dId;

	for (auto& str : mix->paramStrings()) {
		cacheParam(id, str, 0.f);
	}

	cacheType(id, NodeType::QUAD_MIXER);
	cacheType(aId, NodeType::VALUE);
	cacheType(bId, NodeType::VALUE);
	cacheType(cId, NodeType::VALUE);
	cacheType(dId, NodeType::VALUE);
	cacheParam(id, "node_id", id);
	cacheParam(id, "inputa_id", aId);
	cacheParam(id, "inputb_id", bId);
	cacheParam(id, "inputc_id", cId);
	cacheParam(id, "inputd_id", dId);
	return id;
}

int SeqNodeCreator::create()
{
	auto k = std::make_shared<Seq>();
	auto t = std::make_shared<Value>();
	auto reset = std::make_shared<Value>();
	auto tid = m_g->insert_node(t);
	auto rid = m_g->insert_node(reset);
	auto id = m_g->insert_node(k);
	m_g->insert_edge(id, rid);
	m_g->insert_edge(id, tid);
	k->params[Seq::NODE_ID] = id;
	k->params[Seq::TRIGIN_ID] = tid;
	k->params[Seq::RESET_ID] = rid;
	k->params[Seq::TRIGIN] = 0.f;
	cacheType(id, NodeType::SEQ);
	cacheType(tid, NodeType::VALUE);
	cacheType(rid, NodeType::VALUE);
	for (auto& str : k->paramStrings()) {
		cacheParam(id, str, 0.f);
	}
	cacheParam(id, "trigin_id", tid);
	cacheParam(id, "reset_id", rid);
	cacheParam(id, "enable_s1", 1.f);
	cacheParam(id, "enable_s2", 1.f);
	cacheParam(id, "enable_s3", 1.f);
	cacheParam(id, "enable_s4", 1.f);
	cacheParam(id, "enable_s5", 1.f);
	cacheParam(id, "enable_s6", 1.f);
	cacheParam(id, "enable_s7", 1.f);
	cacheParam(id, "enable_s8", 1.f);
	return id;
}

int OscillatorNodeCreator::create()
{
	auto oscNode = std::make_shared<Oscillator>();
	auto oscMod = std::make_shared<Value>(INVALID_PARAM_VALUE);
	auto freq = std::make_shared<Value>(INVALID_PARAM_VALUE);
	auto depthMod = std::make_shared<Value>(0.f);
	auto depthModId = m_g->insert_node(depthMod);
	auto oscModId = m_g->insert_node(oscMod);
	auto freqId = m_g->insert_node(freq);
	auto id = m_g->insert_node(oscNode);
	oscNode->params[Oscillator::MODDEPTH_ID] = depthModId;
	oscNode->params[Oscillator::MODFREQ_ID] = oscModId;
	oscNode->params[Oscillator::FREQ_ID] = freqId;
	oscNode->params[Oscillator::NODE_ID] = id;
	m_g->insert_edge(id, depthModId);
	m_g->insert_edge(id, oscModId);
	m_g->insert_edge(id, freqId);
	// todo: refactor assignment and caching should happen in one call
	cacheType(id, NodeType::OSCILLATOR);
	cacheType(oscModId, NodeType::VALUE);
	cacheType(freqId, NodeType::VALUE);
	cacheType(depthModId, NodeType::VALUE);
	cacheParam(id, "moddepth_id", depthModId);
	cacheParam(id, "modfreq_id", oscModId);
	cacheParam(id, "freq_id", freqId);
	cacheParam(id, "node_id", id);
	cacheParam(id, "freq", 0.f);
	cacheParam(id, "modfreq", 0.f);
	cacheParam(id, "moddepth", 0.f);
	cacheParam(id, "tuning_coarse", 0.f);
	cacheParam(id, "tuning_fine", 0.f);
	cacheParam(id, "waveform", 0.f);

	return id;
}

int OutputNodeCreator::create()
{
	auto outputNode = std::make_shared<Output>();
	auto leftNode = std::make_shared<Value>(0.f);
	auto rightNode = std::make_shared<Value>(0.f);
	auto rightNodeId = m_g->insert_node(rightNode);
	auto leftNodeId = m_g->insert_node(leftNode);
	auto outputId = m_g->insert_node(outputNode);
	m_g->setRoot(outputId);
	m_g->insert_edge(outputId, rightNodeId);
	m_g->insert_edge(outputId, leftNodeId);
	outputNode->params[Output::NODE_ID] = outputId;
	outputNode->params[Output::INPUT_R_ID] = rightNodeId;
	outputNode->params[Output::INPUT_L_ID] = leftNodeId;
	cacheType(outputId, NodeType::OUTPUT);
	cacheType(leftNodeId, NodeType::VALUE);
	cacheType(rightNodeId, NodeType::VALUE);
	cacheParam(outputId, "left_id", leftNodeId);
	cacheParam(outputId, "right_id", rightNodeId);
	cacheParam(outputId, "display_left", 0.f);
	cacheParam(outputId, "display_right", 0.f);
	cacheParam(outputId, "mute", 0.f);

	return outputId;
}

int GainNodeCreator::create()
{
	// process
	auto gainNode = std::make_shared<Gain>();
	// inputs
	auto gainIn = std::make_shared<Value>(0.f);
	auto gainMod = std::make_shared<Value>(1.f);
	auto panMod = std::make_shared<Value>(0.f);
	// outputs
	auto leftNode = std::make_shared<Relay>(0);
	auto rightNode = std::make_shared<Relay>(1);
	// insert inputs -- reverse order
	auto panModId = m_g->insert_node(panMod);
	auto gainModId = m_g->insert_node(gainMod);
	auto gainInId = m_g->insert_node(gainIn);
	// insert process
	auto gainId = m_g->insert_node(gainNode);
	// insert outputs
	auto leftId = m_g->insert_node(leftNode);
	auto rightId = m_g->insert_node(rightNode);
	// assign node ids
	gainNode->params[Gain::NODE_ID] = gainId;
	gainNode->params[Gain::INPUT_ID] = gainInId;
	gainNode->params[Gain::GAINMOD_ID] = gainModId;
	gainNode->params[Gain::LEFT_ID] = leftId;
	gainNode->params[Gain::RIGHT_ID] = rightId;
	// create links
	m_g->insert_edge(gainId, panModId);
	m_g->insert_edge(gainId, gainModId);
	m_g->insert_edge(gainId, gainInId);
	m_g->insert_edge(leftId, gainId);
	m_g->insert_edge(rightId, gainId);
	// cache types
	cacheType(gainId, NodeType::GAIN);
	cacheType(panModId, NodeType::VALUE);
	cacheType(gainInId, NodeType::VALUE);
	cacheType(gainModId, NodeType::VALUE);
	cacheType(leftId, NodeType::RELAY);
	cacheType(rightId, NodeType::RELAY);
	// cache params
	cacheParam(gainId, "input_id", gainInId);
	cacheParam(gainId, "gainmod_id", gainModId);
	cacheParam(gainId, "left_id", leftId);
	cacheParam(gainId, "right_id", rightId);
	cacheParam(gainId, "panmod_id", panModId);
	cacheParam(gainId, "gain", 0.f);
	cacheParam(gainId, "gainmod_depth", 0.f);
	cacheParam(gainId, "panmod_depth", 0.f);
	cacheParam(gainId, "pan", 0.f);
	cacheParam(gainId, "leftout", 0.f);
	cacheParam(gainId, "rightout", 0.f);

	return gainId;
}

int EnvelopeNodeCreator::create()
{
	auto node = std::make_shared<Envelope>();
	// all inputs are Value nodes
	auto in = std::make_shared<Value>();
	auto trig = std::make_shared<Value>();
	// insert input params in reverse order
	auto trig_id = m_g->insert_node(trig);
	auto in_id = m_g->insert_node(in);
	auto id = m_g->insert_node(node);
	// assign input ids in order
	node->params[Envelope::NODE_ID] = id;
	node->params[Envelope::INPUT_ID] = in_id;
	node->params[Envelope::TRIG_ID] = trig_id;
	// insert input edges in reverse order
	m_g->insert_edge(id, trig_id);
	m_g->insert_edge(id, in_id);
	// update cache
	cacheType(id, NodeType::ENVELOPE);
	cacheType(trig_id, NodeType::VALUE);
	cacheType(in_id, NodeType::VALUE);
	cacheParam(id, "node_id", id);
	cacheParam(id, "input_id", in_id);
	cacheParam(id, "trig_id", trig_id);
	cacheParam(id, "attack_ms", 0.f);
	cacheParam(id, "decay_ms", 0.f);
	cacheParam(id, "sustain_db", 0.f);
	cacheParam(id, "release_ms", 0.f);
	cacheParam(id, "trigger", 0.f);
	return id;

}

int DelayNodeCreator::create()
{
	auto node = std::make_shared<Delay>();
	auto in = std::make_shared<Value>(0.f);
	auto inid = m_g->insert_node(in);
	auto id = m_g->insert_node(node);
	node->params[Delay::NODE_ID] = id;
	node->params[Delay::INPUT_ID] = inid;
	m_g->insert_edge(id, inid);
	// update cache
	cacheType(id, NodeType::DELAY);
	cacheType(inid, NodeType::VALUE);
	for (auto& str : node->paramStrings()) {
		cacheParam(id, str, 0.f);
	}
	cacheParam(id, "input_id", inid);
	cacheParam(id, "delay_ms", 1.f);
	cacheParam(id, "modrate_hz", 0.f);
	cacheParam(id, "moddepth_ms", 0.f);
	cacheParam(id, "feedback_highpass_hz", 1000.f);
	cacheParam(id, "feedback_lowpass", 0.7f);
	cacheParam(id, "drywet_ratio", 0.5f);

	return id;
}

int FilterNodeCreator::create()
{
	auto node = std::make_shared<Filter>();
	auto in = std::make_shared<Value>(0.f);
	auto mod = std::make_shared<Value>(0.f);
	auto depth = std::make_shared<Value>(0.f);
	auto dId = m_g->insert_node(depth);
	auto mId = m_g->insert_node(mod);
	auto inId = m_g->insert_node(in);
	auto id = m_g->insert_node(node);
	node->params[Filter::NODE_ID] = id;
	node->params[Filter::INPUT_ID] = inId;
	node->params[Filter::FREQMOD_ID] = mId;
	node->params[Filter::MODDEPTH_ID] = dId;
	m_g->insert_edge(id, dId);
	m_g->insert_edge(id, mId);
	m_g->insert_edge(id, inId);
	cacheType(id, NodeType::FILTER);
	cacheType(inId, NodeType::VALUE);
	cacheType(dId, NodeType::VALUE);
	cacheType(mId, NodeType::VALUE);
	cacheParam(id, "input_id", inId);
	cacheParam(id, "freqmod_id", mId);
	cacheParam(id, "moddepth_id", dId);
	cacheParam(id, "freqmod", 0.f);
	cacheParam(id, "freq", 0.f);
	cacheParam(id, "q", 0.f);
	cacheParam(id, "moddepth", 0.f);

	return id;
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

void NodeCreationCommand::cacheString(int id, std::string param, std::string str) {
	if (m_v.map.find(id) == m_v.map.end()) {
		NodeSnapshot snap;
		m_v.map[id] = snap;
	}
	m_v.map[id].stringParams[param] = str;

}
