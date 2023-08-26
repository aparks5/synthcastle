#pragma once

#include "node.h"
#include <unordered_map>

class QuadMixer : public Node
{
public:
	QuadMixer();
	virtual ~QuadMixer() {}
	float process() override;
	int lookupParam(std::string str) override;
	enum QuadMixerParams {
		NODE_ID,
		INPUT_A_ID,
		INPUT_B_ID,
		INPUT_C_ID,
		INPUT_D_ID,
		INPUT_A,
		INPUT_B,
		INPUT_C,
		INPUT_D,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
		{"inputa_id", INPUT_A_ID},
		{"inputb_id", INPUT_B_ID},
		{"inputc_id", INPUT_C_ID},
		{"inputd_id", INPUT_D_ID},
		{"inputa", INPUT_A},
		{"inputb", INPUT_B},
		{"inputc", INPUT_C},
		{"inputd", INPUT_D}
	};
};