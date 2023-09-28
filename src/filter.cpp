#include "filter.h"

#include "imnodes.h"

Filter::Filter()
	: Node(NodeType::FILTER, "filter", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
	, m_moogFilter(44100)
{

	paramMap = 
	{
		{"node_id", NODE_ID},
		{"input_id", INPUT_ID},
		{"freqmod_id", FREQMOD_ID},
		{"moddepth_id", MODDEPTH_ID},
		{"moddepth", MODDEPTH},
		{"freq", FREQ},
		{"q", Q}
	};

}

void Filter::process() noexcept
{
	m_moogFilter.q(params[Q]);
	auto freq = params[FREQ] +
		(params[FREQ]
			* params[FREQMOD]
			* params[MODDEPTH]);
	m_moogFilter.freq(freq);
	//float out = in;
	//m_moogFilter.apply(&out, 1);
}


