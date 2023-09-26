#pragma once

#include "hilbert.h"
#include "node.h"

class FrequencyShifter : public Node
{
public:
	FrequencyShifter();
	virtual ~FrequencyShifter() {}
	float process() override;

	enum Inputs {
		NUM_INPUTS
	};

	enum Outputs {
		NUM_OUTPUTS
	};

	enum Params {
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