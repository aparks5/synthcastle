#include "output.h"
#include "imnodes.h"

Output::Output()
    : Node(NodeType::PROCESSOR, "output", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
}

void Output::process() noexcept
{

	if (params[MUTE] == 1) {
		outputs[OUTPUT_LEFT] = 0;
		outputs[OUTPUT_RIGHT] = 0;
	}
	else {
		outputs[OUTPUT_LEFT] = inputs[INPUT_LEFT];
		outputs[OUTPUT_RIGHT] = inputs[INPUT_RIGHT];
	}
}

