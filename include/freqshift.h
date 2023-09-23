#pragma once

#include "hilbert.h"
#include "node.h"

class FrequencyShifter : public Node
{
public:
	FrequencyShifter();
	virtual ~FrequencyShifter() {}
	float process() override;

	int lookupParam(std::string str) override {
		return m_lookup[str];
	}
	std::vector<std::string> paramStrings() override
	{
		std::vector<std::string> strings;
		for (std::unordered_map<std::string, int>::iterator iter = m_lookup.begin(); iter != m_lookup.end(); ++iter) {
			auto k = iter->first;
			strings.push_back(k);
		}

		return strings;
	}

	enum FreqShiftParams {
		NODE_ID,
		INPUT_ID,
		INPUT,
		FREQ_ID,
		FREQ,
		MODFREQ,
		DRYWET,
		OUTPUT_ID,
		OUTPUT,
		NUM_PARAMS
	};


private:
	HilbertTransform hilbert;
	size_t m_fs;

	std::unordered_map<std::string, int> m_lookup = {
		{"node_id", NODE_ID},
		{"input_id", INPUT_ID},
		{"input", INPUT},
		{"drywet", DRYWET},
		{"freq_id", FREQ_ID},
		{"freq", FREQ},
		{"output_id", OUTPUT_ID}
	};
	
};