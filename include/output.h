#pragma once

#include "node.h"
#include <unordered_map>

class Output : public Node
{
public:
	Output();
	virtual ~Output() {};
	float process() override;

	enum Inputs {
		INPUT_L_ID,
		INPUT_R_ID,
		INPUT_L,
		INPUT_R,
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT_L,
		OUTPUT_R,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		MUTE,
		DISPLAY_L,
		DISPLAY_R,
		NUM_PARAMS
	};

private:
	std::unordered_map<std::string, int> m_lookup = {
		{"node_id", NODE_ID},
		{"left_id", INPUT_L_ID},
		{"right_id", INPUT_R_ID},
		{"display_left", DISPLAY_L},
		{"display_right", DISPLAY_R},
		{"mute", MUTE}
	};

};
