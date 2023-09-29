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
		IN_LEFT,
		IN_RIGHT,
		NUM_INPUTS
	};

	enum Outputs {
		OUT_LEFT,
		OUT_RIGHT,
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

};
