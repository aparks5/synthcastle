#pragma once

#include "node.h"
#include <unordered_map>

// in evaluate() the PortAudio callback will see this node in the graph and simply push one
// value at a time to the top of the value stack
class AudioInput : public Node
{
public:
	AudioInput();
	virtual ~AudioInput() {};

	enum Inputs {
		NUM_INPUTS
	};

	enum Outputs {
		OUTPUT_ID,
		OUTPUT,
		NUM_OUTPUTS
	};

	enum Params {
		NODE_ID,
		NUM_PARAMS
	};

};
