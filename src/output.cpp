#include "output.h"
#include "imnodes.h"

Output::Output()
    : Node(NodeType::OUTPUT, "output", NUM_INPUTS, NUM_OUTPUTS, NUM_PARAMS)
{
}

float Output::process()
{
	if (params[MUTE] == 1) {
	     return 0;
	}
	// change outputs[OUTPUT] to value?
    return 0;
}

