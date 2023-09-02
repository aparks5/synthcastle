#include "model.h"
#include "util.h"

#include "constant.h"
#include "delay.h"
#include "envelope.h"
#include "filter.h"
#include "gain.h"
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
	m_creators[NodeType::CONSTANT] = std::make_shared<ConstantNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::DELAY] = std::make_shared<DelayNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::ENVELOPE] = std::make_shared<EnvelopeNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::FILTER] = std::make_shared<FilterNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::GAIN] = std::make_shared<GainNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::OSCILLATOR] = std::make_shared<OscillatorNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::OUTPUT] = std::make_shared<OutputNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::QUAD_MIXER] = std::make_shared<MixerNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::SAMPLER] = std::make_shared<SamplerNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::SEQ] = std::make_shared<SeqNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::TRIG] = std::make_shared<TrigNodeCreator>(m_graph, m_cache);
}

const ViewBag Model::refresh()
{
	std::stack<int> postorder;
	if (m_graph.getRoot() != -1) {
		dfs_traverse(m_graph, [&postorder](const int node_id) -> void { postorder.push(node_id); });

		// traverse all nodes and update the cache with all the params
		// after evaluate() process() can update any number of internal params
		// we need to refresh the cache with these in order to reflect them
		// in the UI
		while (!postorder.empty()) {
			const int id = postorder.top();
			postorder.pop();
			const auto& pNode = m_graph.node(id);
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

std::tuple<float,float> Model::evaluate()
{
	m_mut.try_lock();
	
	float clockCache = -1;

	if (m_graph.getRoot() == -1) {
		return {0, 0};
	}

	std::stack<int> postorder;
	dfs_traverse(m_graph, [&postorder](const int node_id) -> void { postorder.push(node_id); });

	std::stack<float> value_stack;
	while (!postorder.empty())
	{
		const int id = postorder.top();
		postorder.pop();
		const auto& pNode = m_graph.node(id);

		switch (pNode->type) {
		case NodeType::DELAY:
		{
			auto val = value_stack.top();
			value_stack.pop();
			value_stack.push(pNode->process(val));
		}
		break;
        case NodeType::FILTER:
        {
            auto in = value_stack.top();
            value_stack.pop();
            auto mod = value_stack.top();
            value_stack.pop();
            auto depth = value_stack.top();
            value_stack.pop();

            pNode->params[Filter::FREQMOD] = mod;
            pNode->params[Filter::MODDEPTH] = depth;
            value_stack.push(pNode->process(in));
        }
        break;
        case NodeType::ENVELOPE:
        {
			// pop off the stack in input order
            auto in = value_stack.top();
            value_stack.pop();
            auto trig = value_stack.top();
            value_stack.pop();
            pNode->params[Envelope::TRIG] = trig;
            value_stack.push(pNode->process(in));
        }
        break;
        case NodeType::SEQ:
        {
			// i should queue this til after eval
			// pop off the stack in input order
            auto trig = value_stack.top();
            value_stack.pop();
            pNode->params[Seq::TRIGIN] = trig;
            auto reset = value_stack.top();
            value_stack.pop();
            pNode->params[Seq::RESET] = reset;

            value_stack.push(pNode->process());
        }
        break;
        case NodeType::QUAD_MIXER:
        {
            auto d = value_stack.top();
            value_stack.pop();
            auto c = value_stack.top();
            value_stack.pop();
            auto b = value_stack.top();
            value_stack.pop();
            auto a = value_stack.top();
            value_stack.pop();
            pNode->params[QuadMixer::INPUT_A] = a;
            pNode->params[QuadMixer::INPUT_B] = b;
            pNode->params[QuadMixer::INPUT_C] = c;
            pNode->params[QuadMixer::INPUT_D] = d;
            value_stack.push(pNode->process());
        }
        break;
		case NodeType::SAMPLER:
		{
			pNode->update();

			auto val = value_stack.top();
			value_stack.pop();
			pNode->params[Sampler::PITCH] = val;

			auto pos = value_stack.top();
			value_stack.pop();
			pNode->params[Sampler::POSITION] = pos;

			auto startstop = value_stack.top();
			value_stack.pop();
			pNode->params[Sampler::STARTSTOP] = startstop;

			value_stack.push(pNode->process());
		}
		break;
        //case NodeType::MIDI_IN:
        //{
        //    // push all voices to stack
        //    value_stack.push(pNode->process());
        //}
        //break;
		case NodeType::OSCILLATOR:
		{
			pNode->update();
			auto freq = value_stack.top();
			value_stack.pop();
			if (freq != INVALID_PARAM_VALUE) {
                // scale midi as float to hz
                float target = 0;
                if ((freq * 128) > 15) {
                    target = midiNoteToFreq((int)(freq * 128.));
                }
                pNode->params[Oscillator::FREQ] = target;
			}
			auto mod = value_stack.top();
			value_stack.pop();
			if (mod != INVALID_PARAM_VALUE) {
				pNode->params[Oscillator::MODFREQ] = mod;
			}
			auto depth = value_stack.top();
			value_stack.pop();
			if (depth != INVALID_PARAM_VALUE) {
				pNode->params[Oscillator::MODDEPTH] = depth;
			}

			auto temp = pNode->process();
			value_stack.push(temp);
		}
		break;
        case NodeType::CONSTANT:
        {
            auto val = pNode->process();
            value_stack.push(val);
        }
        break;
        case NodeType::TRIG:
        {
			if (clockCache == -1) {
				auto val = pNode->process();
				clockCache = val;
				value_stack.push(val);
			}
			else {
				value_stack.push(clockCache);
			}
        }
        break;
		case NodeType::GAIN:
		{
			auto in = value_stack.top();
			value_stack.pop();
			auto mod = value_stack.top();
			value_stack.pop();
			pNode->params[Gain::GAINMOD] = abs(mod);
			auto val = pNode->process(in);
			value_stack.push(val);
		}
		break;
		case NodeType::VALUE:
		{
             //if the edge does not have an edge connecting to another node, then just use the value
            // at this node. it means the node's input pin has not been connected to anything and
            // the value comes from the node's ui.
			if (m_graph.num_edges_from_node(id) == 0ull) {
				value_stack.push(pNode->value);
			}
		}
        break;
        case NodeType::OUTPUT:
        {
			if (!value_stack.empty()) {
				float left = value_stack.top();
				value_stack.pop(); 
				pNode->params[Output::DISPLAY_L] = left;
				// hack for now, mono output, two outputs triggers process() calls twice
				return std::make_tuple(left, left);
                if (value_stack.empty()) {
					m_mut.unlock();
                    return std::make_tuple(left, 0.);
                }
				else {
                    float right = value_stack.top();
                    pNode->params[Output::DISPLAY_R] = right;
                    value_stack.pop(); 
					m_mut.unlock();
					return std::make_tuple(left, right);
                }
			}
		}
		break;
		default:
			break;
		}
	}

	m_mut.unlock();
    return std::make_tuple(0.,0.);
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
	m_mut.try_lock();

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

	m_mut.unlock();
	return 0;
}

int Model::update(UpdateStringEvent update)
{
	m_mut.try_lock();

	auto i = update.nodeId;
	auto p = update.parameter;
	auto v = update.value;
	auto node = m_graph.node(i);
	// todo: restrict to valid values for a given param
	if (node->stringParams.find(p) != node->stringParams.end()) {
		node->stringParams[p] = v;
		m_cache.map[i].stringParams[p] = v;
	}

	m_mut.unlock();
	return 0;
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
	auto k = std::make_shared<Constant>();
	auto id = m_g.insert_node(k);
	k->params[Constant::NODE_ID] = id;
	k->params[Constant::VALUE] = 0.f;
	cacheType(id, NodeType::CONSTANT);
	cacheParam(id, "value", 0.f);
	return id;
}

int SamplerNodeCreator::create()
{
	auto k = std::make_shared<Sampler>();
	auto in = std::make_shared<Value>();
	auto pos = std::make_shared<Value>();
	auto startstop = std::make_shared<Value>();
	auto inid = m_g.insert_node(in);
	auto posid = m_g.insert_node(pos);
	auto startid = m_g.insert_node(startstop);
	auto id = m_g.insert_node(k);
	k->params[Sampler::NODE_ID] = id;
	k->params[Sampler::PITCH_ID] = inid;
	k->params[Sampler::POSITION_ID] = posid;
	k->params[Sampler::STARTSTOP_ID] = startid;
	m_g.insert_edge(id, startid);
	m_g.insert_edge(id, posid);
	m_g.insert_edge(id, inid);
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
	auto id = m_g.insert_node(k);
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
		auto temp = std::make_shared<Relay>();
		auto tempId = m_g.insert_node(temp);
		k->params[Trig::TRIG1_ID + idx - 1] = tempId;
		m_g.insert_edge(tempId, id);
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
	auto aId = m_g.insert_node(a);
	auto bId = m_g.insert_node(b);
	auto cId = m_g.insert_node(c);
	auto dId = m_g.insert_node(d);
	auto mixId = m_g.insert_node(mix);
	m_g.insert_edge(mixId, dId);
	m_g.insert_edge(mixId, cId);
	m_g.insert_edge(mixId, bId);
	m_g.insert_edge(mixId, aId);
	mix->params[QuadMixer::NODE_ID] = mixId;
	mix->params[QuadMixer::INPUT_A_ID] = aId;
	mix->params[QuadMixer::INPUT_B_ID] = bId;
	mix->params[QuadMixer::INPUT_C_ID] = cId;
	mix->params[QuadMixer::INPUT_D_ID] = dId;
	cacheType(mixId, NodeType::QUAD_MIXER);
	cacheType(aId, NodeType::VALUE);
	cacheType(bId, NodeType::VALUE);
	cacheType(cId, NodeType::VALUE);
	cacheType(dId, NodeType::VALUE);
	cacheParam(mixId, "node_id", mixId);
	cacheParam(mixId, "inputa_id", aId);
	cacheParam(mixId, "inputb_id", bId);
	cacheParam(mixId, "inputc_id", cId);
	cacheParam(mixId, "inputd_id", dId);
	cacheParam(mixId, "inputa", 0.f);
	cacheParam(mixId, "inputb", 0.f);
	cacheParam(mixId, "inputc", 0.f);
	cacheParam(mixId, "inputd", 0.f);
	return mixId;
}

int SeqNodeCreator::create()
{
	auto k = std::make_shared<Seq>();
	auto t = std::make_shared<Value>();
	auto reset = std::make_shared<Value>();
	auto tid = m_g.insert_node(t);
	auto rid = m_g.insert_node(reset);
	auto id = m_g.insert_node(k);
	m_g.insert_edge(id, rid);
	m_g.insert_edge(id, tid);
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
	auto depthModId = m_g.insert_node(depthMod);
	auto oscModId = m_g.insert_node(oscMod);
	auto freqId = m_g.insert_node(freq);
	auto id = m_g.insert_node(oscNode);
	oscNode->params[Oscillator::MODDEPTH_ID] = depthModId;
	oscNode->params[Oscillator::MODFREQ_ID] = oscModId;
	oscNode->params[Oscillator::FREQ_ID] = freqId;
	oscNode->params[Oscillator::NODE_ID] = id;
	m_g.insert_edge(id, depthModId);
	m_g.insert_edge(id, oscModId);
	m_g.insert_edge(id, freqId);
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
	auto rightNodeId = m_g.insert_node(rightNode);
	auto leftNodeId = m_g.insert_node(leftNode);
	auto outputId = m_g.insert_node(outputNode);
	m_g.setRoot(outputId);
	m_g.insert_edge(outputId, rightNodeId);
	m_g.insert_edge(outputId, leftNodeId);
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

	return outputId;
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
	cacheType(gainId, NodeType::GAIN);
	cacheType(gainInId, NodeType::VALUE);
	cacheType(gainModId, NodeType::VALUE);
	cacheParam(gainId, "input_id", gainInId);
	cacheParam(gainId, "gainmod_id", gainModId);
	cacheParam(gainId, "gain", 0.f);

	return gainId;
}

int EnvelopeNodeCreator::create()
{
	auto node = std::make_shared<Envelope>();
	// all inputs are Value nodes
	auto in = std::make_shared<Value>();
	auto trig = std::make_shared<Value>();
	// insert input params in reverse order
	auto trig_id = m_g.insert_node(trig);
	auto in_id = m_g.insert_node(in);
	auto id = m_g.insert_node(node);
	// assign input ids in order
	node->params[Envelope::NODE_ID] = id;
	node->params[Envelope::INPUT_ID] = in_id;
	node->params[Envelope::TRIG_ID] = trig_id;
	// insert input edges in reverse order
	m_g.insert_edge(id, trig_id);
	m_g.insert_edge(id, in_id);
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
	auto inid = m_g.insert_node(in);
	auto id = m_g.insert_node(node);
	node->params[Delay::NODE_ID] = id;
	node->params[Delay::INPUT_ID] = inid;
	m_g.insert_edge(id, inid);
	// update cache
	cacheType(id, NodeType::DELAY);
	cacheType(inid, NodeType::VALUE);
	for (auto& str : node->paramStrings()) {
		cacheParam(id, str, 0.f);
	}
	cacheParam(id, "input_id", inid);

	return id;
}

int FilterNodeCreator::create()
{
	auto node = std::make_shared<Filter>();
	auto in = std::make_shared<Value>(0.f);
	auto mod = std::make_shared<Value>(0.f);
	auto depth = std::make_shared<Value>(0.f);
	auto dId = m_g.insert_node(depth);
	auto mId = m_g.insert_node(mod);
	auto inId = m_g.insert_node(in);
	auto id = m_g.insert_node(node);
	node->params[Filter::NODE_ID] = id;
	node->params[Filter::INPUT_ID] = inId;
	node->params[Filter::FREQMOD_ID] = mId;
	node->params[Filter::MODDEPTH_ID] = dId;
	m_g.insert_edge(id, dId);
	m_g.insert_edge(id, mId);
	m_g.insert_edge(id, inId);
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
