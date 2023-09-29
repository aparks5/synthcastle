#include "output.h"
#include "imnodes.h"

Output::Output()
    : Node(NodeType::PROCESSOR, "output", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
	paramMap = {
		{"node_id", NODE_ID},
		{"display_left", DISPLAY_L},
		{"display_right", DISPLAY_R},
		{"mute", MUTE}
	};

	inputMap = {
		{"left", IN_LEFT},
		{"right", IN_RIGHT}
	};

	outputMap = {
		{"left", OUT_LEFT},
		{"right", OUT_RIGHT}
	};

	initIdStrings();
}

void Output::process() noexcept
{

	if (params[MUTE] == 1) {
		outputs[OUT_LEFT] = 0;
		outputs[OUT_RIGHT] = 0;
	}
	else {
		outputs[OUT_LEFT] = inputs[IN_LEFT];
		outputs[OUT_RIGHT] = inputs[IN_RIGHT];
	}
}

