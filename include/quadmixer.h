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

	std::vector<std::string> paramStrings() override
	{
		std::vector<std::string> strings;
		for (std::unordered_map<std::string, int>::iterator iter = m_lookup.begin(); iter != m_lookup.end(); ++iter) {
			auto k = iter->first;
			strings.push_back(k);
		}

		return strings;
	}

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
		GAIN_A,
		GAIN_B,
		GAIN_C,
		GAIN_D,
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
		{"inputd", INPUT_D},
		{"gain_a", GAIN_A},
		{"gain_b", GAIN_B},
		{"gain_c", GAIN_C},
		{"gain_d", GAIN_D},
	};
};