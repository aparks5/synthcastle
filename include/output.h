#pragma once

#include "node.h"
#include <unordered_map>

class Output : public Node
{
public:
	Output();
	virtual ~Output() {};
	void process() noexcept override;

	enum Inputs {
		INPUT_LEFT,
		INPUT_RIGHT,
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT_LEFT,
		OUTPUT_RIGHT,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		INPUT_L_ID,
		INPUT_R_ID,
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
