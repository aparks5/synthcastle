#include "model.h"
#include "util.h"

#include "constant.h"
#include "output.h"
#include "gain.h"
#include "oscillator.h"
#include "value.h"

#define INVALID_PARAM_VALUE (-128)

Model::Model()
{
	m_creators[NodeType::GAIN] = std::make_shared<GainNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::OSCILLATOR] = std::make_shared<OscillatorNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::CONSTANT] = std::make_shared<ConstantNodeCreator>(m_graph, m_cache);
	m_creators[NodeType::OUTPUT] = std::make_shared<OutputNodeCreator>(m_graph, m_cache);
}

std::tuple<float,float> Model::evaluate()
{
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
        //case NodeType::FILTER:
        //{
        //    auto in = value_stack.top();
        //    value_stack.pop();
        //    auto mod = value_stack.top();
        //    value_stack.pop();
        //    auto depth = value_stack.top();
        //    value_stack.pop();

        //    pNode->params[Filter::FREQMOD] = mod;
        //    pNode->params[Filter::MODDEPTH] = depth;
        //    //printf("depth %f, \t mod %f, \t in %f\n", depth, mod, in);
        //    value_stack.push(pNode->process(in));
        //}
        //break;
        //case NodeType::QUAD_MIXER:
        //{
        //    auto d = value_stack.top();
        //    value_stack.pop();
        //    auto c = value_stack.top();
        //    value_stack.pop();
        //    auto b = value_stack.top();
        //    value_stack.pop();
        //    auto a = value_stack.top();
        //    value_stack.pop();
        //    pNode->params[QuadMixer::INPUT_A] = a;
        //    pNode->params[QuadMixer::INPUT_B] = b;
        //    pNode->params[QuadMixer::INPUT_C] = c;
        //    pNode->params[QuadMixer::INPUT_D] = d;
        //    value_stack.push(pNode->process());
        //}
        //break;
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
                if (value_stack.empty()) {
                    return std::make_tuple(left, 0.);
                }

                if (!value_stack.empty()) {
                    float right = value_stack.top();
                    pNode->params[Output::DISPLAY_R] = right;
                    value_stack.pop(); 
					return std::make_tuple(left, right);
                }
			}
		}
		break;
		default:
			break;
		}
	}

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
