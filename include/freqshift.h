#pragma once

#include "hilbert.h"
#include "node.h"

class FrequencyShifter : public Node
{
public:
	FrequencyShifter();
	virtual ~FrequencyShifter() {}
	void process() noexcept override;

	enum Inputs {
		INPUT,
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		INPUT_ID,
		FREQ_ID,
		OUTPUT_ID,
		FREQ,
		MODFREQ,
		DRYWET,
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