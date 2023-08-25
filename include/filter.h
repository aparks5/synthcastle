#pragma once

#include "node.h"
#include <unordered_map>
#include "KrajeskiMoog.h"

class Filter : public Node
{
public:
	Filter();
	virtual ~Filter() {}

	enum FiltParams {
		NODE_ID,
		INPUT_ID,
		FREQMOD_ID,
		FREQMOD,
		MODDEPTH_ID,
		MODDEPTH,
		FREQ,
		Q,
		NUM_PARAMS,
	};

	float process(float in) override;
	int lookupParam(std::string str) override;

private:
	std::unordered_map<std::string, int> m_lookup =
	{
		{"node_id", NODE_ID},
		{"input_id", INPUT_ID},
		{"freqmod_id", FREQMOD_ID},
		{"moddepth_id", MODDEPTH_ID},
		{"moddepth", MODDEPTH},
		{"freq", FREQ},
		{"q", Q}
	};
	KrajeskiMoog m_moogFilter;
	float m_freq;
	float m_q;

};
