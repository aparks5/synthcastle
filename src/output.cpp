#include "output.h"
#include "imnodes.h"

Output::Output()
    : Node(NodeType::OUTPUT, 0., NUM_PARAMS)
{
}

float Output::process()
{
	if (params[MUTE] == 1) {
	     return 0;
	}
    return value;
}

